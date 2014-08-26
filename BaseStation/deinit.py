#!/usr/bin/python

import comm

c=comm.Comm('/dev/ttyUSB0',2400,debug=True)

b=c.wait_for_char()
print "GUN: {0:#04x}".format(b)
c.flush_comms()

print c.get_flash_page(b,0x7F0)

rn=c.get_random_number(b)
c.assign_id(b,rn,0x80)
print "Gun de-initialized."
