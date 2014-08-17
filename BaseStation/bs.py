#!/usr/bin/python

import comm
import serial
import db
import time

c=comm.Comm('/dev/ttyUSB0',2400,debug=True);
#c=comm.SimComm(debug=True);
#c=comm.SimComm2(debug=True);
d=db.RAMDatabase()

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
	#res=c.set_power(gun,0x10)
	#if res is None:
	#	print "failed to set power"
	#else:
	#	print "set power OK!"

while True:
	b=c.wait_for_char()
	if b & 0x80:
		hit_by(b)
