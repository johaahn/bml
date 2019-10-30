# -*- coding: utf-8 -*-
# Copyright (c) 2013, Johann Baudy. All rights reserved.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301 USA

import bml
import socket
import sys
import errno

class bml_buffer(bml.writer, bml.parser):
    """!@brief BML Buffer Class.
        This class allow to write and read of BML node inside a buffer
        """

    def __init__(self):
        """!@brief Constructor of BML buffer.
            """
        bml.writer.__init__(self)
        bml.parser.__init__(self)

        self.buffer = []


    def io_write(self, in_data):
        """!@brief IO write callback of BML buffer.

            @param in_data data to write inside buffer.
            """
        self.buffer.append(in_data)


    def io_read(self, in_sz_data, in_first_header):
        """!@brief IO read callback of BML buffer.

            @param in_sz_data number of bytes to read from buffer.
            @param in_first_header first header flag allow callback to exit on new data.
            """
        ## TODO
        print("TODO")


class bml_file(bml.writer, bml.parser):
    """!@brief BML File Class.
    This class allow to write and read of BML node inside a file
    """
    def __init__(self, filename, mode):
        """!@brief Constructor of BML file.

            @param filename path to file.
            @param mode open for reading 'r' or writing 'w'.
            """
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
            if data != b'BML\x00':
                raise Exception("No header found")
        elif mode == "w":
            self.fd = open(filename, "wb")
            self.fd.write(bytes("BML\0", 'UTF-8'));



    def close(self):
        """!@brief Close file.
        """
        self.fd.close()
        self.fd = None

    def io_write(self, in_data):
        """!@brief IO write callback of BML file.

            @param in_data data to write inside file.
            """
        #print "write " +str(len(in_data)) + " bytes :" + in_data.encode('hex')
        try:
            self.fd.write(in_data)
            return 0
        except Exception as e:
            print(msg)
            self.fd.close()
            self.fd = None
            return -1

    def io_read(self, in_sz_data, in_first_header):
        """!@brief IO read callback of BML file.

            @param in_sz_data number of bytes to read from file.
            @param in_first_header first header flag allow callback to exit on new data.
            """
        #print("receive request " +str(in_sz_data) + " bytes (Timeoutable:" + str(in_first_header)+")")
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
                except Exception as e:
                    print("ERROR DONTWAIT:" + str(e))
                    sys.exit(-1)

                #print "Received "+str(nbytes) + " bytes"

                if nbytes == 0:
                    if in_first_header:
                        #print "TIMEOUT"
                        # Timeout
                        return ""
                    else:
                        print("ERROR NODATA")
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

"""!@brief BML Socket Base Class.
   This class allow to write and read of BML node inside a socket (TCP, UDP, ...)
   """
class bml_socket(bml.writer, bml.parser):

    def __init__(self, socket):
        """!@brief Constructor of BML socket.

            @param socket python socket.
            """
        bml.writer.__init__(self)
        bml.parser.__init__(self)
        self.sock = socket
        self.read_sz_tmp = 384*1024*1024
        self.read_data_tmp = bytearray(self.read_sz_tmp)
        self.read_view_tmp = memoryview(self.read_data_tmp)
        self.read_size_tmp = 0
        self.read_start_tmp = 0

    def io_write(self, in_data):
        """!@brief IO write callback of BML socket.

            @param in_data data to write to socket.
            """
        #return 0
        #print"write " +str(len(in_data)) + " bytes :" + in_data.encode('hex')
        #print(type(in_data))
        #print(type(in_data.encode()))
        #print(':'.join(hex(ord(x))[2:] for x in 'Hello World!'))
        try:
            self.sock.sendall(in_data)
            return 0
        except socket.error as msg:
            print(msg)
            self.sock.close()
            self.sock = None

            return -1

    def io_read(self, in_sz_data, in_first_header):
        """!@brief IO read callback of BML socket.

            @param in_sz_data number of bytes to read from socket.
            @param in_first_header first header flag allow callback to exit on new data.
            """
        print("receive request " +str(in_sz_data) + " bytes (Timeoutable:" + str(in_first_header)+")")
        sz_remaining = in_sz_data

        while 1:
            #update view read

            print("LOOP BEGIN - Received: "+str(self.read_size_tmp)+", Remaining: "+str(sz_remaining)+ ", Offset:" + str(self.read_start_tmp))
            if self.read_size_tmp < in_sz_data:
                view_update = self.read_view_tmp[self.read_start_tmp+self.read_size_tmp:]
                print(len(view_update))
                # If not enough room, moving buffer to beginning
                if len(view_update) < sz_remaining:
                    if self.read_size_tmp:
                        view_read = self.read_view_tmp[self.read_start_tmp:(self.read_start_tmp+self.read_size_tmp)]
                        print("MEMMOVE "+str(len(view_read)))
                        #memmove(self.read_view_tmp, view_read, len(view_read))
                        #self.read_view_tmp[0:self.read_size_tmp] =
                        self.read_view_tmp[0:self.read_size_tmp] = view_read.tobytes()
                        #print "dd"

                    self.read_start_tmp = 0
                    view_update = self.read_view_tmp[self.read_start_tmp+self.read_size_tmp:]


                # assert enough buffer for sz_remaining
                assert(len(view_update) >= sz_remaining)


                print("Start recv_into "+str(len(view_update)) + " bytes")
                try:
                    nbytes = self.sock.recv_into(view_update, len(view_update), flags=socket.MSG_DONTWAIT)
                    print("END recv_into "+str(nbytes) + " bytes")
                except socket.error as msg:
                    if msg.errno != errno.EAGAIN:
                        print("ERROR DONTWAIT:" + str(msg))
                        sys.exit(-1)
                    else:
                        nbytes = 0
                print("Received "+str(nbytes) + " bytes")

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
                            print("ERROR WAIT:" + str(msg))
                            sys.exit(-1)
                        print("Received "+str(nbytes) + " bytes (WAIT)")
                #increment counters
                self.read_size_tmp += nbytes
                sz_remaining -= min(nbytes, sz_remaining)

                print("Received: "+str(self.read_size_tmp)+", Remaining: "+str(sz_remaining)+ ", Offset:" + str(self.read_start_tmp))
            else:
                assert(in_sz_data <= self.read_size_tmp)
                view_data = self.read_view_tmp[self.read_start_tmp:self.read_start_tmp+in_sz_data]
                self.read_start_tmp += in_sz_data
                self.read_size_tmp -= in_sz_data
                print("Extract: "+str(in_sz_data)+"bytes")
                return view_data.tobytes()


class bml_tcp_socket(bml_socket):
    """!@brief BML TCP Socket class.
        This class allow to write and read of BML node inside a TCP socket
        """
    def __init__(self):
        """!@brief Constructor of BML TCP socket.
            """
        print("Creating bml_tcp_socket")
        # Initialize base class
        bml_socket.__init__(self, None)

        # Create socket
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error as msg:
            print("Unable to create socket")
            print(msg)
            sys.exit(-1)

    def __del__(self):
        """!@brief Destructor of BML TCP socket.
            """
        self.disconnect()

    def connect(self,ip, port):
        """!@brief Connect TCP Socket to a TCP server.
            """
        print("Trying to connect to "+ip+":"+str(port))
        self.sock.connect((ip, port))



    def disconnect(self):
        """!@brief Disconnect TCP socket.
            """
        if self.sock:
            self.sock.close()
            self.sock = None

def enum(offset, *sequential, **named):
    enums = dict(zip(sequential, range(offset,offset+len(sequential))), **named)
    reverse = dict((value, key) for key, value in enums.items())
    enums['reverse_mapping'] = reverse
    return type('Enum', (), enums)
