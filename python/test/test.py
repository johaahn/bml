import sys, os
sys.path.append('./')
import struct
import socket
from bml_tools import bml_file, bml_socket, bml_tcp_socket, bml, enum

a = bml.node(10)
dat =struct.pack('BBB', 10, 11, 35);
print(dat)
print(type(dat))
print(type(a))
print(type(a.data))
print(type(b'dsdsfdsf'))
print('DATA:',a.data)
a.data = [0,1,2]


# TEST CHILD CREATION

b = bml.node(12)
b.data = b"trop cool 12";
a.append(b);
b = bml.node(11)
b.data = b"trop cool 11";
a.append(b);

print(a.id)
print(b.data)
print(a.data)

# TEST CHILD ACCESS

print(a.get(11).data)
print(a.get(12).data)


# Write to file
wr = bml_file('/tmp/test.bml','w');
a.to_writer(wr,0);
wr.close()

# Read from file
rd = bml_file('/tmp/test.bml','r');
ar = bml.node();
ar.from_parser(rd);

#try :
#    print(a.get(13).data)
#    print("Should failed ...")
#    sys.error(-1)
#except bml.exception as e:
#    print(e.message)
#    print("ok")

# TEST WRITER
class wr_test(bml.writer):
    def io_write(self, in_data):
        print("write " +str(len(in_data)) + " bytes :" + str(in_data.tobytes()))
        return 0
wr = wr_test();
a.to_writer(wr,0);


# TEST LOOPBACK

# Opening Unix socket (Other sockets can be used, ex:TCP, UDP, ...)
s1, s2 = socket.socketpair()
client1 = bml_socket(s1);
client2 = bml_socket(s2);

# Write BML node to a socket
try:
    a.to_writer(client1,0);
except bml.exception as e:
    print(e.message)
    sys.exit(-1)

# Get first node on other side of socket
ar = bml.node();
ar.from_parser(client2);

print(ar.get(11).data)
print(ar.get(12).data)

# Get second node on other side of socket
ar = bml.node();
ar.from_parser(client2);
print(ar.data)

#print writer.data;
