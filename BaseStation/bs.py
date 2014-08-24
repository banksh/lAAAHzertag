#!/usr/bin/python

import comm
import serial
import db
import time
import gun
import graphics
import Tkinter

c=comm.Comm('/dev/ttyUSB0',2400,debug=True);
#c=comm.SimComm(debug=True);
#c=comm.SimComm2(debug=True);
d=db.RAMDatabase()

BG={
'idle':'#660000',
'talk':'#000099',
'success':'#006600'
}

DEFAULT_TEXT="Laser Tag Base Station"

fsg=graphics.FullScreenGraphics()

cv=Tkinter.Canvas(fsg,width=fsg.width,height=fsg.height,highlightthickness=0,bg=BG['idle'])
cv.pack()

coord = fsg.center()
arc = cv.create_text(coord, text=DEFAULT_TEXT, fill="white", justify=Tkinter.CENTER, font=("Helvetica",72))

txt = Tkinter.Entry(cv,justify=Tkinter.CENTER,font=("Helvetica",72),highlightthickness=0,border=0,bg=BG['talk'],fg='white')
w=cv.create_window((coord[0],coord[1]+120),window=txt)

cv.itemconfig(w,state=Tkinter.HIDDEN)

flag = False

def txt_done(event):
	global flag
	flag=True

txt.bind('<Return>',txt_done)

fsg.show()

def set_text(txt):
	cv.itemconfig(arc,text=txt)

def get_text_input():
	global flag
	txt.delete(0,Tkinter.END)
	cv.itemconfig(w,state=Tkinter.NORMAL)
	txt.focus()
	flag=False
	while not flag:
		time.sleep(0.1)	
	cv.itemconfig(w,state=Tkinter.HIDDEN)
	return txt.get()

def graphic_success():
	cv.config(bg=BG['success'])
	set_text("Good to go!")
	time.sleep(1)
	cv.config(bg=BG['idle'])
	set_text(DEFAULT_TEXT)

def new_gun(gun_id):
	rn=c.get_random_number(gun_id)
	new_gun_id=d.new_gun_id()
	c.assign_id(gun_id,rn,new_gun_id)
	d.confirm_new_gun_id()

	set_text("Welcome! Enter your name:")
	name=get_text_input()
	d.add_new_name(new_gun_id,name)
	set_text("Fire your gun again to finish.")

def init_gun(gun_id,config):
	config[gun.CONFIG_ID]=gun_id
	config[gun.CONFIG_HEALTH]=16
	config[gun.CONFIG_RESPAWN_DELAY]=100
	config[gun.CONFIG_SHIELD]=2
	config[gun.CONFIG_DEATH_PERIOD]=3000
	config[gun.CONFIG_FIRE_THRESHOLD]=500
	config[gun.CONFIG_FIRE_CHEATING]=2000
	config[gun.CONFIG_FIRE_HOLDOFF]=1000

	c.set_flash_page(gun_id,gun.FLASH_CONFIG,config)
	c.success(gun_id)
	graphic_success()

def retrieve_hitlist(gun_id):
	hitlist = c.get_flash_page(gun_id,gun.FLASH_HITLIST)
	hitlist=[(b & 0x7F | 0x80, (b >> 7) | 0x80) for b in hitlist]
	hitlist = [item for sublist in hitlist for item in sublist]+[128]
	hitlist=hitlist[0:hitlist.index(128)]
	if len(hitlist):
		c.set_flash_page(gun_id,gun.FLASH_HITLIST,[0]*16)
	return hitlist

def talk(gun_id,config):
	config[gun.CONFIG_HEALTH] = 16
	config[gun.CONFIG_FIRE_THRESHOLD]=500
	config[gun.CONFIG_FIRE_HOLDOFF]=1000
	config[gun.CONFIG_FIRE_HOLDOFF]=1000

	print "Hitlist:",retrieve_hitlist(gun_id)
	c.set_flash_page(gun_id,gun.FLASH_CONFIG,config)
	c.success(gun_id)
	graphic_success()

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
				set_text("Hello, {0}\nSyncing...".format(gun_name))
				talk(gun_id,config)
	except comm.CommFailed:
		print "Failed."
		set_text(DEFAULT_TEXT)
		pass

try:
	while True:
		cv.config(bg=BG['idle'])
		b=c.wait_for_char()
		if b & 0x80:
			cv.config(bg=BG['talk'])
			set_text("Busy...")
			hit_by(b)
except Exception:
	fsg.close()
	raise

