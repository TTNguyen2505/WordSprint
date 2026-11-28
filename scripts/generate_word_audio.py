"""Generate pronunciation audio and update the application seed paths."""

import asyncio
from contextlib import closing
import re
import sqlite3
import tempfile
from pathlib import Path, PurePosixPath

import edge_tts


ROOT = Path(__file__).resolve().parents[1]
VOICE = "en-US-AriaNeural"
SQL_STRING = r"'(?:''|[^'])*'"
WORD_ROW = re.compile(
    rf"(?P<prefix>^[ \t]*\([ \t]*(?P<id>\d+)[ \t]*,[ \t]*\d+[ \t]*,"
    rf"[ \t]*{SQL_STRING}[ \t]*,[ \t]*(?P<word>{SQL_STRING})[ \t]*,[ \t]*)"
    rf"(?P<audio>NULL|{SQL_STRING})(?P<suffix>[ \t]*,[ \t]*{SQL_STRING}[ \t]*\))",
    re.MULTILINE | re.IGNORECASE,
)


def sql_value(value):
    return value[1:-1].replace("''", "'")


def audio_destination(root, word, existing_path):
    if existing_path is None:
        if not re.fullmatch(r"[A-Za-z0-9]+(?:[ -][A-Za-z0-9]+)*", word):
            raise ValueError(f"Cannot safely derive an audio filename for {word!r}")
        filename = re.sub(r"[ -]+", "-", word.lower()) + ".mp3"
        existing_path = f"storage/audio/{filename}"
    relative = PurePosixPath(existing_path)
    if (
        relative.is_absolute()
        or "\\" in existing_path
        or ".." in relative.parts
        or relative.parts[:2] != ("storage", "audio")
        or relative.suffix.lower() != ".mp3"
    ):
        raise ValueError(f"Invalid audio path for {word!r}: {existing_path!r}")
    destination = root.joinpath(*relative.parts).resolve()
    if not destination.is_relative_to((root / "storage" / "audio").resolve()):
        raise ValueError(f"Audio path escapes storage/audio: {existing_path!r}")
    return existing_path, destination


def read_seed(root):
    seed_path = root / "backend" / "sql" / "seed_app_data.sql"
    source = seed_path.read_bytes()
    text = source.decode("utf-8")
    with closing(sqlite3.connect(":memory:")) as database:
        database.executescript((root / "backend" / "sql" / "schema.sql").read_text(encoding="utf-8"))
        database.executescript(text)
        rows = database.execute(
            "SELECT word_id, target_word, audio_path FROM word_pool ORDER BY word_id"
        ).fetchall()
    if not rows:
        raise ValueError("No words found in the application seed.")

    matches = list(WORD_ROW.finditer(text))
    if len(matches) != len(rows):
        raise ValueError("Could not safely locate every word_pool row in the seed SQL.")
    by_id = {word_id: (word, path) for word_id, word, path in rows}
    destinations = {}
    updates = {}
    for match in matches:
        word_id = int(match.group("id"))
        audio = match.group("audio")
        old_path = None if audio.upper() == "NULL" else sql_value(audio)
        word = sql_value(match.group("word"))
        if by_id.pop(word_id, None) != (word, old_path):
            raise ValueError(f"Seed row {word_id} does not match the loaded word pool.")
        path, destination = audio_destination(root, word, old_path)
        if destination in destinations and destinations[destination] != word:
            raise ValueError(f"Different words share the audio path {path!r}.")
        destinations[destination] = word
        updates[word_id] = path
    if by_id:
        raise ValueError("Some seeded words could not be located in the SQL.")
    return seed_path, source, text, matches, updates, destinations


def validate_mp3(data, path):
    if len(data) < 1024 or not (
        data.startswith(b"ID3") or (data[0] == 0xFF and data[1] & 0xE0 == 0xE0)
    ):
        raise ValueError(f"Invalid or incomplete MP3: {path}")


async def synthesize(word):
    chunks = []
    async for chunk in edge_tts.Communicate(word, VOICE).stream():
        if chunk["type"] == "audio":
            chunks.append(chunk["data"])
    return b"".join(chunks)


async def generate_audio(root=ROOT, synthesize_word=synthesize):
    seed_path, source, text, matches, updates, destinations = read_seed(root)
    for path, word in destinations.items():
        if path.exists():
            validate_mp3(path.read_bytes(), path)
            print(f"Skipping existing audio: {path}", flush=True)
            continue
        print(f"Generating audio for {word!r}...", flush=True)
        data = await synthesize_word(word)
        validate_mp3(data, path)
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open("xb") as output:
            try:
                output.write(data)
            except OSError:
                output.close()
                path.unlink(missing_ok=True)
                raise

    pieces = []
    last = 0
    for match in matches:
        pieces.append(text[last:match.start("audio")])
        pieces.append("'" + updates[int(match.group("id"))].replace("'", "''") + "'")
        last = match.end("audio")
    pieces.append(text[last:])
    updated = "".join(pieces).encode("utf-8")
    if updated != source:
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(dir=seed_path.parent, delete=False) as output:
                temporary = Path(output.name)
                output.write(updated)
            if seed_path.read_bytes() != source:
                raise RuntimeError("The seed file changed during generation; audio paths were not updated.")
            temporary.replace(seed_path)
        finally:
            if temporary is not None:
                temporary.unlink(missing_ok=True)
    elif seed_path.read_bytes() != source:
        raise RuntimeError("The seed file changed during generation; audio paths were not updated.")
    print(f"Audio paths ready for {len(updates)} words.", flush=True)


if __name__ == "__main__":
    asyncio.run(generate_audio())
