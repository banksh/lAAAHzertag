#!/usr/bin/python

import comm

#c=comm.Comm('/dev/ttyS0',1200);
c=comm.SimComm(debug=True);

def hit_by(gun):
	print "Hello, ",hex(gun)
	res=c.set_power(gun,0x10)
	if res is None:
		print "failed to set power"
	else:
		print "set power OK!"

while True:
	if c.has_char():
		b=c.get_char()
		if b & 0x80:
			hit_by(b)
