import bml
import socket
import sys

class bml_file(bml.writer, bml.parser):
	def __init__(self, filename, mode):
		bml.writer.__init__(self)
		bml.parser.__init__(self)
		
		self.read_sz_tmp = 512*1024*1024
		self.read_data_tmp = bytearray(self.read_sz_tmp)
		self.read_view_tmp = memoryview(self.read_data_tmp)
		self.read_size_tmp = 0
		self.read_start_tmp = 0

		if mode == "r":
			self.fd = open(filename, "rb")
			data = self.fd.read(4);
			if data != "BML\0":
				raise Exception("No header found")
		elif mode == "w":
			self.fd = open(filename, "wb")
			self.fd.write("BML\0");
			
	def close(self):
		self.fd.close()
		self.fd = None

	def io_write(self, in_data):
		#print "write " +str(len(in_data)) + " bytes :" + in_data.encode('hex')
		try:
			self.fd.write(in_data)
			return 0
		except Exception, e:
			print msg
			self.fd.close()
			self.fd = None
			return -1

	def io_read(self, in_sz_data, in_first_header):
		#print "receive request " +str(in_sz_data) + " bytes (Timeoutable:" + str(in_first_header)+")"
		sz_remaining = in_sz_data 
		
		while 1:
			#update view read
			
			#print "LOOP BEGIN - Received: "+str(self.read_size_tmp)+", Remaining: "+str(sz_remaining)+ ", Offset:" + str(self.read_start_tmp)
			if self.read_size_tmp < in_sz_data:
				view_update = self.read_view_tmp[self.read_start_tmp+self.read_size_tmp:]
				#print len(view_update)
				# If not enough room, moving buffer to beginning
				if len(view_update) < sz_remaining:
					if self.read_size_tmp:
						view_read = self.read_view_tmp[self.read_start_tmp:(self.read_start_tmp+self.read_size_tmp)]
						#print "MEMMOVE "+str(len(view_read))
						#memmove(self.read_view_tmp, view_read, len(view_read))
						#self.read_view_tmp[0:self.read_size_tmp] = 
						self.read_view_tmp[0:self.read_size_tmp] = view_read.tobytes()
						#print "dd"

					self.read_start_tmp = 0
					view_update = self.read_view_tmp[self.read_start_tmp+self.read_size_tmp:]
					

				# assert enough buffer for sz_remaining
				assert(len(view_update) >= sz_remaining)


				
				try:
					nbytes = self.fd.readinto(view_update)
				except Exception , e:
					print "ERROR DONTWAIT:" + str(e)
					sys.exit(-1)
				
				#print "Received "+str(nbytes) + " bytes"

				if nbytes == 0:
					if in_first_header:
						#print "TIMEOUT"
						# Timeout						
						return ""
					else:
						print "ERROR NODATA"
						sys.exit(-1)

						#print "Received "+str(nbytes) + " bytes (WAIT)"
				#increment counters
				self.read_size_tmp += nbytes
				sz_remaining -= min(nbytes, sz_remaining)
				
				#print "Received: "+str(self.read_size_tmp)+", Remaining: "+str(sz_remaining)+ ", Offset:" + str(self.read_start_tmp)
			else:
				assert(in_sz_data <= self.read_size_tmp)
				view_data = self.read_view_tmp[self.read_start_tmp:self.read_start_tmp+in_sz_data]
				self.read_start_tmp += in_sz_data
				self.read_size_tmp -= in_sz_data
				#print "Extract: "+str(in_sz_data)+"bytes"
				return view_data.tobytes()

class bml_socket(bml.writer, bml.parser):
	def __init__(self, socket):
		bml.writer.__init__(self)
		bml.parser.__init__(self)
		self.sock = socket
		self.read_sz_tmp = 384*1024*1024
		self.read_data_tmp = bytearray(self.read_sz_tmp)
		self.read_view_tmp = memoryview(self.read_data_tmp)
		self.read_size_tmp = 0
		self.read_start_tmp = 0

	def io_write(self, in_data):
		#print "write " +str(len(in_data)) + " bytes :" + in_data.encode('hex')
		try:
			self.sock.sendall(in_data)
			return 0
		except socket.error as msg:
			print msg
			self.sock.close()
			self.sock = None

			return -1

	def io_read(self, in_sz_data, in_first_header):
		#print "receive request " +str(in_sz_data) + " bytes (Timeoutable:" + str(in_first_header)+")"
		sz_remaining = in_sz_data 
		
		while 1:
			#update view read
			
			#print "LOOP BEGIN - Received: "+str(self.read_size_tmp)+", Remaining: "+str(sz_remaining)+ ", Offset:" + str(self.read_start_tmp)
			if self.read_size_tmp < in_sz_data:
				view_update = self.read_view_tmp[self.read_start_tmp+self.read_size_tmp:]
				#print len(view_update)
				# If not enough room, moving buffer to beginning
				if len(view_update) < sz_remaining:
					if self.read_size_tmp:
						view_read = self.read_view_tmp[self.read_start_tmp:(self.read_start_tmp+self.read_size_tmp)]
						#print "MEMMOVE "+str(len(view_read))
						#memmove(self.read_view_tmp, view_read, len(view_read))
						#self.read_view_tmp[0:self.read_size_tmp] = 
						self.read_view_tmp[0:self.read_size_tmp] = view_read.tobytes()
						#print "dd"

					self.read_start_tmp = 0
					view_update = self.read_view_tmp[self.read_start_tmp+self.read_size_tmp:]
					

				# assert enough buffer for sz_remaining
				assert(len(view_update) >= sz_remaining)

				try:
					nbytes = self.sock.recv_into(view_update, len(view_update), flags=socket.MSG_DONTWAIT)
				except socket.error as msg:
					if msg[0] != 11:
						print "ERROR DONTWAIT:" + str(msg)
						sys.exit(-1)
					else:
						nbytes = 0
				#print "Received "+str(nbytes) + " bytes"

				if nbytes == 0:
					if in_first_header:
						#print "TIMEOUT"
						# Timeout						
						return ""
					else:
						# Try blocking read
						try:
							nbytes = self.sock.recv_into(view_update, sz_remaining)
						except socket.error as msg:
							print "ERROR WAIT:" + str(msg)
							sys.exit(-1)
						#print "Received "+str(nbytes) + " bytes (WAIT)"
				#increment counters
				self.read_size_tmp += nbytes
				sz_remaining -= min(nbytes, sz_remaining)
				
				#print "Received: "+str(self.read_size_tmp)+", Remaining: "+str(sz_remaining)+ ", Offset:" + str(self.read_start_tmp)
			else:
				assert(in_sz_data <= self.read_size_tmp)
				view_data = self.read_view_tmp[self.read_start_tmp:self.read_start_tmp+in_sz_data]
				self.read_start_tmp += in_sz_data
				self.read_size_tmp -= in_sz_data
				#print "Extract: "+str(in_sz_data)+"bytes"
				return view_data.tobytes()


class bml_tcp_socket(bml_socket):
	def __init__(self):
		print "Creating bml_tcp_socket"
		# Initialize base class
		bml_socket.__init__(self, None)
		
		# Create socket
		try:
			self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		except socket.error as msg:
			print "Unable to create socket"
			print msg
			sys.exit(-1)

	def __del__(self):
		self.disconnect()
		
	def connect(self,ip, port):
		print "Trying to connect to "+ip+":"+str(port)
		self.sock.connect((ip, port))
	

		
	def disconnect(self):
		if self.sock:
			self.sock.close()
			self.sock = None

def enum(offset, *sequential, **named):
    enums = dict(zip(sequential, range(offset,offset+len(sequential))), **named)
    reverse = dict((value, key) for key, value in enums.iteritems())
    enums['reverse_mapping'] = reverse
    return type('Enum', (), enums)

