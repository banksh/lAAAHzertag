#!/usr/bin/python
import sqlite3 as sq
import pickle

class Database:
	def __init__(self):
		self.conn = sq.connect('guns.db')
		self.c = self.conn.cursor()
	def new_gun_id(self):
		m = self.c.execute('SELECT MAX(gun_id) FROM guns').fetchone()
		return max(0x81, (m[0] or 0) + 1)
	def confirm_id(self, val):
		self.c.execute("INSERT INTO guns VALUES({0}, '', '', '')".format(int(val)))
		self.conn.commit()
	def add_name(self, gun_id, name):
		self.c.execute("UPDATE guns SET athena=? WHERE gun_id=?", (name, gun_id,))
	def get_name_from_gun(self, gun_id):
		return self.c.execute("SELECT athena FROM guns WHERE gun_id=?", (gun_id,)).fetchone()[0]
	def dump_db(self):
		return self.c.execute("SELECT * from guns")
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
	n = db.new_gun_id()
	db.confirm_id(n)
	db.add_name(n, "maxj")
	print db.get_name_from_gun(n)
	for row in db.dump_db():
		print row
	db.close()
	
