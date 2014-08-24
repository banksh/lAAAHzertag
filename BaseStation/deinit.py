#!/usr/bin/python

import comm

c=comm.Comm('/dev/ttyUSB0',2400,debug=True)

location=0x7F0
dry_run=False

def deinit(gun_id):
	page=list(c.get_flash_page(gun_id,location))
	print "Location: {0:#06x}".format(location)
	print "Read:",["{0:#06x}".format(word) for word in page]

	page[0]=0x80

	print "Write:",["{0:#06x}".format(word) for word in page]
	if not dry_run:
		c.set_flash_page(gun_id,location,page)
	print "Done."

b=c.wait_for_char()
print hex(b)
deinit(b)


