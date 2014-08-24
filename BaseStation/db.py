#!/usr/bin/python
import sqlite3 as sq
import pickle

class Database:
	def __init__(self):
		self.conn = sq.connect('guns.db')
		self.c = self.conn.cursor()
		self.blacklist = [0xFF, 0xE0]
	def new_gun_id(self):
		m = self.c.execute('SELECT MAX(gun_id) FROM guns').fetchone()
		v = m[0] or 0
		while max([0x81, v + 1]) in self.blacklist:
			v += 1
		assert v < 0xFF
		return v + 1
	def confirm_id(self, val):
		self.c.execute("INSERT INTO guns VALUES(?, '', '', '')", (int(val),))
		self.conn.commit()
	def add_name(self, gun_id, name):
		self.c.execute("UPDATE guns SET athena=? WHERE gun_id=?", (name, gun_id,))
		self.conn.commit()
	def get_name_from_gun(self, gun_id):
		return self.c.execute("SELECT athena FROM guns WHERE gun_id=?", (gun_id,)).fetchone()[0]
	def dump_db(self):
		return self.c.execute("SELECT * from guns")
	def read_config(self, gun_id):
		p = self.c.execute("SELECT config from guns WHERE gun_id=?",(gun_id,)).fetchone()[0]
		return pickle.loads(p)
	def set_config(self, gun_id, config):
		self.c.execute("UPDATE guns SET config=? WHERE gun_id=?", (pickle.dumps(config),gun_id,))
		self.conn.commit()
	def read_hits(self, gun_id):
		p = self.c.execute("SELECT hits from guns WHERE gun_id=?",(gun_id,)).fetchone()[0]
		return pickle.loads(p)
	def set_hits(self, gun_id, hits):
		self.c.execute("UPDATE guns SET hits=? WHERE gun_id=?", (pickle.dumps(hits), gun_id,))
		self.conn.commit()
	def close(self):
		self.conn.close()

class RAMDatabase:
	def __init__(self):
		self.gun_id=0x81
		self.names={}

	def new_gun_id(self):
		return self.gun_id

	def confirm_new_gun_id(self):
		self.gun_id += 1

	def add_new_name(self,gun_id,name):
		self.names[gun_id]=name

	def get_name_from_gun(self,gun_id):
		if gun_id in self.names:
			return self.names[gun_id]
		else:
			return None
if __name__ == '__main__':
	db = Database()
	db.blacklist.append(149)

	n = db.new_gun_id()
	db.confirm_id(n)
	db.add_name(n, "maxj")
	db.set_config(n, {"PowerLevel":9000})
	db.set_hits(n, [0x81])

	print db.read_config(n)
	print db.read_hits(n)
	print db.get_name_from_gun(n)

	db.close()
	
