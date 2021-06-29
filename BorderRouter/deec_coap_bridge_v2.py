#!/usr/bin/env python

import sys
import getopt
import threading
import time
import socket

__author__ = 'Paulo Peixoto'

localListenIPv6 = "fd00::1"
localListenIPv4 = "0.0.0.0"
localListenPort = 5683
bufferSize  = 1024

remoteCoAPServerIP = socket.gethostbyname("coap.me")
remoteCoAPServerPort = 5683

localCoAPClient = "fd00::212:4b00:89ab:cdef"
localCoAPPort = 5683

def IPv6ServerThread(name):
	print "IPv6 Server Thread Starting"
	# Listen for incoming datagrams
	while(True):

		bytesAddressPair = UDPIPv6ServerSocket.recvfrom(bufferSize)
		message = bytesAddressPair[0]
		address = bytesAddressPair[1]

		clientMsg = "Message from Client:{}".format(message)
		clientIP  = "Client IP Address:{}".format(address)
		localCoAPClient=address[0]
		localCoAPPort = address[1]

		time.sleep(0.25)
		# Forward Packet Into Remote Server
		UDPIPv4ServerSocket.sendto(message, (remoteCoAPServerIP, remoteCoAPServerPort))

		print ("Message From {0}:{1} - {2}".format(localCoAPClient,localCoAPPort,message))

def IPv4ServerThread(name):
	print "IPv4 Server Thread Starting"
	# Listen for incoming datagrams
	while(True):

		bytesAddressPair = UDPIPv4ServerSocket.recvfrom(bufferSize)
		message = bytesAddressPair[0]
		address = bytesAddressPair[1]

		clientMsg = "Message from Client:{}".format(message)
		clientIP  = "Client IP Address:{}".format(address)

		time.sleep(0.25)
		# Forward Packet Into Remote Server
		UDPIPv6ServerSocket.sendto(message, (localCoAPClient, localCoAPPort))

		print ("Message From {0}:{1} - {2}".format(address[0],address[1],message))

def parseArguments(argv):
	global remoteCoAPServerIP
	global remoteCoAPServerPort

	try:
		opts, args = getopt.getopt(argv,"hi:p:",["ip=","port="])
	except getopt.GetoptError:
		print 'deec_coap_bridge_v2.py -i <CoAP Server IP> -p <CoAP Server Port>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
	 		print 'deec_coap_bridge_v2.py -i <CoAP Server IP> -p <CoAP Server Port>'
	 		sys.exit()
		elif opt in ("-i", "--ip"):
	 		remoteCoAPServerIP = arg
		elif opt in ("-p","--port"):
	 		remoteCoAPServerPort = int(arg)

	print 'Remote CoAP Server IP:', remoteCoAPServerIP
	print 'Remote CoAP Server Port:', remoteCoAPServerPort

if __name__ == "__main__":
	# Parse Command Line Arguments
	parseArguments(sys.argv[1:])

	# Create a datagram socket
	UDPIPv6ServerSocket = socket.socket(family=socket.AF_INET6, type=socket.SOCK_DGRAM) # UDP
	# Bind to address and ip
	UDPIPv6ServerSocket.bind((localListenIPv6, localListenPort))

	# Create a datagram socket
	UDPIPv4ServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM) # UDP
	# Bind to address and ip
	UDPIPv4ServerSocket.bind((localListenIPv4, localListenPort))


	print "DEEC CoAP Bridge Up and Listening..."

	try:
		IPv6Thread = threading.Thread(target=IPv6ServerThread, args=(1,))
		IPv6Thread.daemon = 1
		IPv6Thread.start()

		IPv4Thread = threading.Thread(target=IPv4ServerThread, args=(1,))
		IPv4Thread.daemon = 1
		IPv4Thread.start()

		# Listen for incoming datagrams
		while(True):
			time.sleep(1)

	except (KeyboardInterrupt): #catch keyboard interrupts
		sys.exit()

