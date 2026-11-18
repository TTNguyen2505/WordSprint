import sqlite3, os, sys

def run_sql_file(conn, path):
    with open(path, 'r', encoding='utf-8') as f:
        sql = f.read()
    conn.executescript(sql)

def main():
    db_path = os.path.abspath(os.path.join(os.path.dirname(__file__), 'vocab.db'))
    # remove if exists
    if os.path.exists(db_path):
        os.remove(db_path)
    conn = sqlite3.connect(db_path)
    try:
        schema_path = os.path.join(os.path.dirname(__file__), 'schema.sql')
        seed_path = os.path.join(os.path.dirname(__file__), 'seed.sql')
        run_sql_file(conn, schema_path)
        run_sql_file(conn, seed_path)
        conn.commit()
        # Count per level
        cur = conn.cursor()
        cur.execute('SELECT l.code, COUNT(v.id) FROM vocabularies v JOIN levels l ON v.level_id = l.id GROUP BY l.code ORDER BY l.id')
        rows = cur.fetchall()
        for code, cnt in rows:
            print(f"Level {code}: {cnt} words")
    finally:
        conn.close()
        # Clean up DB file
        if os.path.exists(db_path):
            os.remove(db_path)

if __name__ == '__main__':
    main()
