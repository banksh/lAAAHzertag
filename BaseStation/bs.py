#!/usr/bin/python

import comm
import serial
import db
import time

c=comm.Comm('/dev/ttyUSB0',2400,debug=True);
#c=comm.SimComm(debug=True);
#c=comm.SimComm2(debug=True);
d=db.RAMDatabase()

CONFIG_ID=0
CONFIG_POWER=1
CONFIG_HEALTH=2
CONFIG_SHIELD=3
CONFIG_RESPAWN_DELAY=4
CONFIG_FIRE_THRESHOLD=5
CONFIG_FIRE_CHEATING=6
CONFIG_FIRE_HOLDOFF=7
CONFIG_DEATH_PERIOD=8

CONFIG_ID_INIT_FLAG = 1<<9

def new_gun(gun):
	print "New gun!"
	rn=c.get_random_number(gun)
	if rn is None:
		print "Failed to initialize new gun."
		return
	new_gun_id=d.new_gun_id()
	if not c.assign_id(gun,rn,new_gun_id):
		print "Failed to initialize new gun."
		return
	d.confirm_new_gun_id()
	print "Enter your name:"
	name=raw_input()
	d.add_new_name(new_gun_id,name)
	print "Fire your gun again to finish."

def hit_by(gun):
	c.flush_comms()
	if gun == 0x80:
		new_gun(gun)
		return

	print "Hello, ",d.get_name_from_gun(gun)
	config=c.get_flash_page(gun,0x7D0)
	if config is not None:
		#print "Config:",config
		if config[CONFIG_ID] & CONFIG_ID_INIT_FLAG:
			config=(gun, 0, 2, 2, 100, 500, 1000, 8000, 3000, 16383, 16383, 16383, 16383, 16383, 16383, 16383)
			if c.set_flash_page(gun,0x7D0,config) and c.success(gun):
				print "Your gun is ready!"
		elif config[CONFIG_HEALTH] == 0:
			print "You were dead!"
			config=(gun, 0, 2, 2, 100, 500, 1000, 8000, 3000, 16383, 16383, 16383, 16383, 16383, 16383, 16383)
			if c.set_flash_page(gun,0x7D0,config) and c.success(gun):
				print "Revived."
		else:
			hitlist = c.get_flash_page(gun,0x7E0)
			if hitlist is not None:
				hitlist=[(b & 0x7F | 0x80, (b >> 7) | 0x80) for b in c.get_flash_page(gun,0x7E0)]
				hitlist = [item for sublist in hitlist for item in sublist]+[128]
				hitlist=hitlist[0:hitlist.index(128)]
				if len(hitlist):
					if c.set_flash_page(gun,0x7E0,[0]*16) and c.success(gun):
						print "Hitlist:",[hex(h) for h in hitlist]
				else:
					if c.success(gun):
						print "Good to go!"

while True:
	b=c.wait_for_char()
	if b & 0x80:
		hit_by(b)
