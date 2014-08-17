#!/usr/bin/python

import comm

c=comm.Comm('/dev/ttyUSB0',2400)

def rd():
	print "Byte to send:",
	s=raw_input()
	if len(s) > 2:
		if s[0:2]=='0x':
			return int(s[2:],16)
	return int(s)

while True:
	c.put_char(rd())
