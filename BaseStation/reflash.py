#!/usr/bin/python

import comm

c=comm.Comm('/dev/ttyUSB0',2400,debug=True)

location=0x760
dry_run=True
gun_id=0x96

page=list(c.get_flash_page(gun_id,location))
print "Location: {0:#06x}".format(location)
print "Read:",["{0:#06x}".format(word) for word in page]

page[5]=0x3067

print "Write:",["{0:#06x}".format(word) for word in page]
if not dry_run:
	c.set_flash_page(gun_id,location,page)
print "Done."
