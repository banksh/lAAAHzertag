#!/usr/bin/python

import comm
import serial
import db
import time
import gun

c=comm.Comm('/dev/ttyUSB0',2400,debug=True);
#c=comm.SimComm(debug=True);
#c=comm.SimComm2(debug=True);
d=db.RAMDatabase()

def new_gun(gun_id):
	rn=c.get_random_number(gun_id)
	new_gun_id=d.new_gun_id()
	c.assign_id(gun_id,rn,new_gun_id)
	d.confirm_new_gun_id()

	print "Welcome! Enter your name:"
	name=raw_input()
	d.add_new_name(new_gun_id,name)
	print "Fire your gun again to finish."

def init_gun(gun_id,config):
	config[gun.CONFIG_ID]=gun_id
	config[gun.CONFIG_HEALTH]=2

	c.set_flash_page(gun_id,gun.FLASH_CONFIG,config)
	c.success(gun_id)
	print "Ready to go!"

def retrieve_hitlist(gun_id):
	hitlist = c.get_flash_page(gun_id,gun.FLASH_HITLIST)
	hitlist=[(b & 0x7F | 0x80, (b >> 7) | 0x80) for b in hitlist]
	hitlist = [item for sublist in hitlist for item in sublist]+[128]
	hitlist=hitlist[0:hitlist.index(128)]
	if len(hitlist):
		c.set_flash_page(gun_id,gun.FLASH_HITLIST,[0]*16)
	return hitlist

def talk(gun_id,config):
	config[gun.CONFIG_HEALTH] = 2

	print "Hitlist:",retrieve_hitlist(gun_id)
	c.set_flash_page(gun_id,gun.FLASH_CONFIG,config)
	c.success(gun_id)

#default_config=(gun_id, 0, 2, 2, 100, 500, 1000, 8000, 3000, 16383, 16383, 16383, 16383, 16383, 16383, 16383)

def hit_by(gun_id):
	c.flush_comms()
	try:
		if gun_id == gun.ID_UNINIT:
			new_gun(gun_id)
		else:
			gun_name=d.get_name_from_gun(gun_id)
			config=list(c.get_flash_page(gun_id,gun.FLASH_CONFIG))
			if config[gun.CONFIG_ID] & gun.CONFIG_ID_INIT_FLAG:
				init_gun(gun_id,config)
			else:
				print "Hello, ",gun_name
				talk(gun_id,config)
	except comm.CommFailed:
		print "Failed."
		pass

while True:
	b=c.wait_for_char()
	if b & 0x80:
		hit_by(b)

