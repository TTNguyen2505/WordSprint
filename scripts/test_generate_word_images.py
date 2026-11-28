import base64
import io
import json
from pathlib import Path
import sqlite3
import tempfile
import unittest
from unittest.mock import Mock, patch

from PIL import Image

import generate_word_images as generator


def image_bytes(image_format):
    output = io.BytesIO()
    Image.new("RGB", (2, 2), "red").save(output, format=image_format)
    return output.getvalue()


class GenerateWordImagesTest(unittest.TestCase):
    def test_seed_images_cover_every_word(self):
        images = generator.seeded_images(generator.ROOT)
        with sqlite3.connect(":memory:") as database:
            for name in ("schema.sql", "seed_app_data.sql"):
                database.executescript(
                    (generator.ROOT / "backend" / "sql" / name).read_text(encoding="utf-8")
                )
            rows = database.execute("SELECT target_word, hint_image_path FROM word_pool").fetchall()
        self.assertEqual(
            set(images), {(word, generator.ROOT / path) for word, path in rows}
        )

    def test_generate_converts_to_png_and_skips_existing(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "images" / "hello.png"
            with patch.object(generator, "generate_image", return_value=image_bytes("JPEG")) as generate:
                generator.generate_images("account", "token", [("hello", path)])
                generator.generate_images("account", "token", [("hello", path)])
            generate.assert_called_once_with("account", "token", "hello")
            with Image.open(path) as image:
                self.assertEqual(image.format, "PNG")
                image.verify()

    def test_cloudflare_response_is_decoded(self):
        data = image_bytes("PNG")
        response = Mock()
        response.__enter__ = Mock(return_value=io.BytesIO(json.dumps({
            "success": True, "result": {"image": base64.b64encode(data).decode("ascii")}
        }).encode()))
        response.__exit__ = Mock(return_value=False)
        with patch.object(generator, "urlopen", return_value=response) as urlopen:
            self.assertEqual(generator.generate_image("account", "token", "hello"), data)
            request = urlopen.call_args.args[0]
            self.assertEqual(request.get_header("Authorization"), "Bearer token")
            self.assertIn("account/ai/run/", request.full_url)

    def test_missing_image_is_an_error(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "hello.png"
            response = Mock()
            response.__enter__ = Mock(return_value=io.BytesIO(b'{"success": true, "result": {}}'))
            response.__exit__ = Mock(return_value=False)
            with patch.object(generator, "urlopen", return_value=response):
                with self.assertRaisesRegex(RuntimeError, "no image"):
                    generator.generate_images("account", "token", [("hello", path)])
            self.assertFalse(path.exists())

    def test_rejects_seed_path_outside_images(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            sql = root / "backend" / "sql"
            sql.mkdir(parents=True)
            (sql / "schema.sql").write_text(
                "CREATE TABLE word_pool(word_id, target_word, hint_image_path);",
                encoding="utf-8",
            )
            (sql / "seed_app_data.sql").write_text(
                "INSERT INTO word_pool VALUES (1, 'hello', 'storage/images/../../hello.png');",
                encoding="utf-8",
            )
            with self.assertRaisesRegex(ValueError, "Invalid image path"):
                generator.seeded_images(root)

    def test_missing_key_fails_before_client_creation(self):
        with patch.dict("os.environ", {}, clear=True), patch.object(generator, "urlopen") as urlopen:
            self.assertEqual(generator.main(), 1)
            urlopen.assert_not_called()


if __name__ == "__main__":
    unittest.main()