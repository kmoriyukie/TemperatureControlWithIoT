#!/usr/bin/env python

import sys
import getopt
import threading
import time
import socket

localListen = "fd00::1"
localListen2 = "aaaa::1"
defaultPort = 5683
bufferSize  = 1024

CoAPServerIP = "aaaa::212:4b00:89ab:cdef"
ClientIP = "fd00::c30c:0:0:2"

def FirstThread(name):
	global ClientIP
	print "Thread #1 Starting"
	# Listen for incoming datagrams
	while(True):

		bytesAddressPair = mySocket.recvfrom(bufferSize)
		message = bytesAddressPair[0]
		address = bytesAddressPair[1]

		clientMsg = "Message from Client:{}".format(message)
		clientIP  = "Client IP Address:{}".format(address)
		ClientIP = address[0]

		time.sleep(0.25)
		# Forward Packet Into Remote Server
		mySocket2.sendto(message, (CoAPServerIP, defaultPort))
		print ("Process 1:")
		print ("Message From {0}:{1} - {2}".format(address[0],address[1],message))

def SecondThread(name):
	print "Thread #2 Starting"
	# Listen for incoming datagrams
	while(True):

		bytesAddressPair = mySocket2.recvfrom(bufferSize)
		message = bytesAddressPair[0]
		address = bytesAddressPair[1]

		clientMsg = "Message from Client:{}".format(message)
		clientIP  = "Client IP Address:{}".format(address)

		time.sleep(0.25)
		# Forward Packet Into Remote Server
		mySocket.sendto(message, (ClientIP, defaultPort))
		print ("Process 2:")
		print ("Message From {0}:{1} - {2}".format(address[0],address[1],message))

if __name__ == "__main__":
	mySocket = socket.socket(family=socket.AF_INET6, type=socket.SOCK_DGRAM) # UDP
	mySocket.bind((localListen, defaultPort))

	mySocket2 = socket.socket(family=socket.AF_INET6, type=socket.SOCK_DGRAM) # UDP
	mySocket2.bind((localListen2, defaultPort))

	print "CoAP Bridge Up and Listening..."

	try:
		IPv6Thread = threading.Thread(target=FirstThread, args=(1,))
		IPv6Thread.daemon = 1
		IPv6Thread.start()

		IPv4Thread = threading.Thread(target=SecondThread, args=(1,))
		IPv4Thread.daemon = 1
		IPv4Thread.start()

		# Listen for incoming datagrams
		while(True):
			time.sleep(1)

	except (KeyboardInterrupt): #catch keyboard interrupts
		sys.exit()

