import sqlite3 as sq
import os, sys

if raw_input("This will wipe out the existing database. Type y to confirm: ") != "y":
    sys.exit(0)

os.system('rm guns.db')
conn = sq.connect('guns.db')
c = conn.cursor()
c.execute('''CREATE TABLE guns
             (gun_id integer, athena text, config blob, hits blob)''')
conn.commit()
conn.close()

print "Done."
