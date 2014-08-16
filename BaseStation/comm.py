#!/usr/bin/python

import serial

class CommException(Exception):
	pass

class Comm:
	CB_SOF = 0x10
	CB_EOF = 0x11

	CMD_ACK = 0x00
	CMD_SET_POWER = 0x08

	CMD_LENGTHS = {
		CMD_ACK: 0,
	}

	def __init__(self,port,baud,timeout=1000,debug=False):
		self.s=serial.Serial(port,baud,timeout=timeout)
		self.debug=debug

	def has_char(self):
		return self.s.inWaiting() > 0

	def put_char(self,d):
		self.s.write(chr(d))
		self.s.flush()

	def get_char(self):
		return ord(self.s.read())

	def start_of_frame(self):
		self.put_char(self.CB_SOF)

	def end_of_frame(self):
		self.put_char(self.CB_EOF)

	def send_data_byte(self,b):
		x=b & 0xF
		y=b >> 4
		self.put_char(x)
		self.put_char(y)

	def assert_start_of_frame(self):
		b=self.get_char()
		if b != self.CB_SOF:
			raise CommException("Did not receive SOF: {0:#04x}".format(b))

	def assert_end_of_frame(self):
		b=self.get_char()
		if b != self.CB_EOF:
			raise CommException("Did not receive EOF: {0:#04x}".format(b))

	def read_data_byte(self):
		x=self.get_char()
		if x & (~0x0F):
			raise CommException("Invalid nibble: {0:#04x}".format(x))
		y=self.get_char()
		if y & (~0x0F):
			raise CommException("Invalid nibble: {0:#04x}".format(y))
		return x | y << 4

	def send_packet(self,to,cmd,params=[]):
		packet=[to,cmd]+params
		checksum=((-sum(packet)) % 256 + 256) % 256
		packet.append(checksum)

		self.start_of_frame()
		for b in packet:
			self.send_data_byte(b)
		self.end_of_frame()

	def recv_cmd(self):
		self.assert_start_of_frame()
		gun_id=self.read_data_byte()
		cmd=self.read_data_byte()
		if cmd not in self.CMD_LENGTHS:
			raise CommException("Invalid command: {0:#04x}".format(cmd))
		l=self.CMD_LENGTHS[cmd]
		params=[self.read_data_byte() for i in range(l)]
		checksum=self.read_data_byte()
		self.assert_end_of_frame()
		if (gun_id+cmd+sum(params)+checksum) % 256:
			raise CommException("Bad checksum: {0:#04x}".format(checksum))
		return (gun_id,cmd,params)

	def flush_comms(self):
		if self.debug:
			print "Flushing comm..."
		try:
			while True:
				self.get_char()
		except serial.SerialTimeoutException:
			pass
		print "Comm flushed."

	def communicate(self,gun_id,send_cmd,send_params=[],recv_cmd=CMD_ACK):
		tries=3
		for i in range(tries):
			self.send_packet(gun_id,send_cmd,send_params)
			try:
				(from_id,recvd_cmd,recv_params)=self.recv_cmd()
				if from_id != gun_id:
					raise CommException("Got response from wrong gun: {0:#04x}".format(from_id))
				if recvd_cmd != recv_cmd:
					raise CommException("Received unexpected response: {0:#04x}".format(recvd_cmd))
			except serial.SerialTimeoutException:
				if self.debug:
					print "TRY {0}: TIMEOUT".format(i+1)
			except CommException as e:
				if self.debug:
					print "TRY {0}: {1}".format(i+1,str(e))
			else:
				break
			self.flush_comms()
			recv_params=None
		return recv_params

	def set_power(self,gun_id,power):
		return self.communicate(gun_id,self.CMD_SET_POWER,[power])

class SimComm(Comm):
	def __init__(self,debug=False):
		self.debug=debug

	def get_char(self):
		s=raw_input()
		if len(s) == 0:
			raise serial.SerialTimeoutException()
		if len(s) > 2:
			if s[0:2]=='0x':
				return int(s[2:],16)
		return int(s)

	def put_char(self,c):
		print "{0:#04x}".format(c)

	def has_char(self):
		return True

