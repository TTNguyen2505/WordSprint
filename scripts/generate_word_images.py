"""Generate vocabulary hint images from the application seed data."""

import base64
import binascii
import io
import json
import os
from pathlib import Path, PurePosixPath
import sqlite3
import sys
from urllib.error import HTTPError, URLError
from urllib.parse import quote
from urllib.request import Request, urlopen

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
MODEL = "@cf/black-forest-labs/flux-1-schnell"
SCENES = {
    "hello": "one person waves a friendly greeting to another person",
    "message": "a person sends a speech bubble from a phone to another person's phone",
    "explain": "a teacher points to a simple diagram while a student listens",
    "negotiate": "two people discuss a deal across a table and shake hands",
    "goodbye": "a person waves farewell to a friend departing on a train",
    "please": "a child politely asks an adult for a glass of water with hands together",
    "thanks": "a person smiles gratefully and receives a gift from a friend",
    "friend": "two friends smile and walk together",
    "name": "a person points to a blank name badge on their shirt",
    "invite": "a person offers a party invitation to a friend",
    "answer": "a student raises a hand to respond to a teacher's question",
    "conversation": "two people chat face to face with blank speech bubbles",
    "suggest": "a person proposes an idea represented by a light bulb to a friend",
    "promise": "two people make a pinky promise",
    "discuss": "a group of people exchange ideas around a table",
    "mention": "one person briefly points out a distant house to another person",
    "apologize": "a person shows regret after accidentally spilling a friend's drink",
    "express": "a person shows their feelings with a happy face and open arms",
    "opinion": "one person shares their view with a group using an empty speech bubble",
    "persuade": "a person convinces a hesitant friend to join a picnic",
    "clarify": "a person clears up confusion by pointing to a simple clear diagram",
    "emphasize": "a teacher points emphatically to one highlighted item on a board",
    "interpret": "a person helps another understand a map by pointing to its symbols",
    "resolve": "two people settle a disagreement and shake hands",
    "smile": "a person smiles happily at a friend",
    "wave": "a person raises a hand and waves to a friend",
    "meet": "two people meet for the first time and shake hands",
    "talk": "two people talk face to face",
    "welcome": "a person opens a door and warmly welcomes a guest",
    "greet": "two people greet each other with a handshake",
    "listen": "a person listens attentively to a friend speaking",
    "reply": "one person replies to another in a conversation",
    "describe": "a person describes a colorful ball to a friend with hand gestures",
    "request": "a person politely asks a librarian for a book",
    "respond": "a person responds to another person's raised hand",
    "communicate": "two people share ideas using gestures and speech bubbles",
    "inform": "a person points to a simple weather forecast to inform a friend",
    "introduce": "a person introduces two friends to each other",
    "convince": "a person convinces a hesitant friend to join a picnic",
    "propose": "a person proposes an idea to colleagues at a table",
    "summarize": "a person points to a few simple pictures that sum up a longer story",
    "acknowledge": "a person nods to show they recognize a friend's contribution",
    "recommend": "a librarian recommends a book to a reader",
    "compromise": "two people sitting across a table shake hands and smile after reaching an agreement",
}


def seeded_images(root):
    with sqlite3.connect(":memory:") as database:
        database.executescript((root / "backend" / "sql" / "schema.sql").read_text(encoding="utf-8"))
        database.executescript((root / "backend" / "sql" / "seed_app_data.sql").read_text(encoding="utf-8"))
        rows = database.execute(
            "SELECT target_word, hint_image_path FROM word_pool ORDER BY word_id"
        ).fetchall()

    image_root = (root / "storage" / "images").resolve()
    paths = {}
    for word, image_path in rows:
        relative = PurePosixPath(image_path)
        if (
            relative.is_absolute()
            or "\\" in image_path
            or ".." in relative.parts
            or relative.parts[:2] != ("storage", "images")
            or relative.suffix.lower() != ".png"
        ):
            raise ValueError(f"Invalid image path for {word!r}: {image_path!r}")
        destination = root.joinpath(*relative.parts).resolve()
        if not destination.is_relative_to(image_root):
            raise ValueError(f"Image path escapes storage/images: {image_path!r}")
        if destination in paths and paths[destination] != word:
            raise ValueError(f"Image path is shared by different words: {image_path!r}")
        paths[destination] = word
    return [(word, path) for path, word in paths.items()]


def generate_image(account_id, token, word):
    scene = SCENES.get(word, f"illustrate the meaning of the English word {word!r}")
    prompt = (
        f"A colorful flat illustration: {scene}. "
        "Bold outlines, simple shapes, square composition, plain background."
    )
    url = (
        "https://api.cloudflare.com/client/v4/accounts/"
        f"{quote(account_id, safe='')}/ai/run/{MODEL}"
    )
    request = Request(
        url,
        data=json.dumps({"prompt": prompt}).encode("utf-8"),
        headers={"Authorization": f"Bearer {token}", "Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urlopen(request, timeout=120) as response:
            result = json.load(response)
    except HTTPError as error:
        raise RuntimeError(f"Cloudflare request for {word!r} failed: HTTP {error.code}") from error
    if not result.get("success"):
        raise RuntimeError(f"Cloudflare request for {word!r} failed: {result.get('errors')}")
    encoded = result.get("result", {}).get("image")
    if not isinstance(encoded, str) or not encoded:
        raise RuntimeError(f"Cloudflare returned no image for {word!r}")
    try:
        return base64.b64decode(encoded, validate=True)
    except binascii.Error as error:
        raise RuntimeError(f"Cloudflare returned invalid image data for {word!r}") from error


def generate_images(account_id, token, images):
    for word, path in images:
        if path.exists():
            with Image.open(path) as image:
                if image.format != "PNG":
                    raise ValueError(f"Existing image is not a PNG: {path}")
                image.verify()
            print(f"Skipping existing image: {path}", flush=True)
            continue

        print(f"Generating image for {word!r}...", flush=True)
        image_data = generate_image(account_id, token, word)
        with Image.open(io.BytesIO(image_data)) as image:
            png = io.BytesIO()
            image.save(png, format="PNG")
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open("xb") as output:
            output.write(png.getvalue())
        print(f"Saved {path}", flush=True)


def main():
    account_id = os.environ.get("CLOUDFLARE_ACCOUNT_ID")
    token = os.environ.get("CLOUDFLARE_API_TOKEN")
    if not account_id or not token:
        print("Error: CLOUDFLARE_ACCOUNT_ID and CLOUDFLARE_API_TOKEN must be set.", file=sys.stderr)
        return 1
    try:
        images = seeded_images(ROOT)
        if not images:
            raise ValueError("No seeded word images found.")
        generate_images(account_id, token, images)
    except (OSError, sqlite3.Error, ValueError, RuntimeError, URLError) as error:
        print(f"Error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())