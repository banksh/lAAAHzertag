#!/usr/bin/python

class Database:
	def __init__(self):
		pass

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
