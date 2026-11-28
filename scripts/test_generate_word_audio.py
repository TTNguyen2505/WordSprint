import asyncio
from contextlib import closing
from pathlib import Path
import shutil
import sqlite3
import tempfile
import unittest

import generate_word_audio as generator


MP3 = b"\xff\xfb" + b"\x00" * 2048


class GenerateWordAudioTest(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        sql = self.root / "backend" / "sql"
        sql.mkdir(parents=True)
        for name in ("schema.sql", "seed_app_data.sql"):
            shutil.copyfile(generator.ROOT / "backend" / "sql" / name, sql / name)
        self.seed = sql / "seed_app_data.sql"

    def rows(self):
        with closing(sqlite3.connect(":memory:")) as database:
            database.executescript((self.seed.parent / "schema.sql").read_text(encoding="utf-8"))
            database.executescript(self.seed.read_text(encoding="utf-8"))
            return database.execute(
                "SELECT word_id, target_word, audio_path FROM word_pool ORDER BY word_id"
            ).fetchall()

    def test_generates_all_words_and_updates_only_audio_paths(self):
        before = self.rows()
        spoken = []

        async def synthesize(word):
            spoken.append(word)
            return MP3

        asyncio.run(generator.generate_audio(self.root, synthesize))
        after = self.rows()
        self.assertEqual([(i, word) for i, word, _ in before],
                         [(i, word) for i, word, _ in after])
        self.assertEqual(len(spoken), len(after))
        for _, _, path in after:
            self.assertTrue(path.startswith("storage/audio/"))
            self.assertEqual((self.root / path).read_bytes(), MP3)
        original_sql = self.seed.read_bytes()
        asyncio.run(generator.generate_audio(self.root, synthesize))
        self.assertEqual(self.seed.read_bytes(), original_sql)
        self.assertEqual(len(spoken), len(after))

    def test_failure_does_not_update_seed(self):
        before = self.seed.read_bytes()
        attempted = []

        async def fail(word):
            attempted.append(word)
            if len(attempted) == 2:
                raise RuntimeError(f"Could not synthesize {word}")
            return MP3

        with self.assertRaisesRegex(RuntimeError, "Could not synthesize"):
            asyncio.run(generator.generate_audio(self.root, fail))
        self.assertEqual(self.seed.read_bytes(), before)
        self.assertEqual((self.root / "storage" / "audio" / "hello.mp3").read_bytes(), MP3)

    def test_invalid_existing_audio_is_not_overwritten(self):
        path = self.root / "storage" / "audio" / "hello.mp3"
        path.parent.mkdir(parents=True)
        path.write_bytes(b"invalid")
        before = self.seed.read_bytes()
        with self.assertRaisesRegex(ValueError, "Invalid or incomplete MP3"):
            asyncio.run(generator.generate_audio(self.root))
        self.assertEqual(path.read_bytes(), b"invalid")
        self.assertEqual(self.seed.read_bytes(), before)

    def test_rejects_audio_path_outside_storage(self):
        with self.assertRaisesRegex(ValueError, "Invalid audio path"):
            generator.audio_destination(self.root, "hello", "storage/audio/../../hello.mp3")


if __name__ == "__main__":
    unittest.main()
