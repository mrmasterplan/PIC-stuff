# good quick python guide
#http://www.angelfire.com/tx4/cus/notes/python.html

import socket
import array

# socket read/write timeout in seconds
TIMEOUT = 10

# the port number to listen on ... needs to match that used in RR interface
SERVER_PORT = 5050

# max read ahead bytes when getting a variable ...
DATA_BUFFER = 1024

# create a temp buffer for the largest possible image
newPixels = array.array('B', [0 for i in range(1280*960*3)])

############################# Image processing ##################################

# Performs the image conversion/processing/etc that you want to perform
def ProcessImage(imageData):

	pixels = imageData.pixels
	width = imageData.width
	height = imageData.height

	# remember that we're working in BGR color ... so the actual width is 3x
	w=width*3

	# sanity check
	if ((pixels=='') or (width==0) or (height==0)): return

	# you could use imageData.name to perform different analysis in the same program ....

	# as an example ... swap red and blue values and store in newPixels buffer
	for i in range(0, w*height, 3):
		# normal image format is BGR ... first byte is blue
		# swap red and blue
		newPixels[i] = ord(pixels[i+2])
		newPixels[i+1] = ord(pixels[i+1])
		newPixels[i+2] = ord(pixels[i])

######################### Socket Class ############################

class MySocket:
	def __init__(self, sock):
		self.sock = sock
		#only wait TIMEOUT seconds for any request
		sock.settimeout(TIMEOUT)

	def write(self, msg, msgLen):
		totalsent = 0
		# keep sending while there is more data to send
		while totalsent < msgLen:
			sent = self.sock.send(msg[totalsent:msgLen])
			if sent == 0:
				raise RuntimeError, "socket connection broken"
			totalsent = totalsent + sent

	def read(self, msgLen):
		msg = ""
		max = 0
		while (max < msgLen):
			chunk = self.sock.recv(msgLen-max)
			if chunk == '':
					raise RuntimeError, "socket connection broken"
			msg = msg + chunk
			max = max + len(chunk)
		return msg

	def close(self):
			self.sock.close()

######################### Image Data Structure ############################

# holds the image data received from RoboRealm
class ImageDataType:
  # hold the name used in RR to identify this module
  name = ''
  # image processed count ... sent back to RR
  count = 0
  # dimensions of the received image
  width = 0
  height = 0
  # holds the image data
  pixels = ''

######################## Data Handling ###############################

#converts from buffer binary object to ordinal/integer number
def byteToInt(data):
    return ord(data[0])+(ord(data[1])<<8)+(ord(data[2])<<16)+(ord(data[3])<<24)

#converts from an integer into a binary byte array
def intToByte(num):
    return chr(num&255)+chr((num>>8)&255)+chr((num>>16)&255)+chr((num>>24)&255)

# returns an error message to RoboRealm. This message is displayed in the "messages" list within
# the RR Pipe Program interface.
def ReturnError(channel, txt):
    channel.write(intToByte(5), 4)
    channel.write("error", 5);
    channel.write(intToByte(len(txt)),4)
    channel.write(txt, len(txt))

# returns a byte string variable to RR. The returned variables can be used
# elsewhere in RR for continued processing.
def ReturnBytesVariable(channel, name, data, dataLen):
		channel.write(intToByte(len(name)), 4)
		channel.write(name, len(name))
		channel.write(intToByte(dataLen), 4)
		channel.write(data, dataLen)

# returns an int variable to RR
def ReturnIntVariable(channel, name, num):
    strTmp = str(num)
    channel.write(intToByte(len(name)), 4)
    channel.write(name, len(name))
    channel.write(intToByte(len(strTmp)), 4)
    channel.write(strTmp, len(strTmp))

######################## Data Processing ##############################

# Parses the variables sent by RR into the appropriate structure. You can add
# your own processing routines here to handle other variables that may get sent.
def ProcessVariable(channel, imageData, name, data, dlen):
    # determine what we've got
    name = name.lower()
    if (name == "name"):
        imageData.name = name
    elif (name == "width"):
        imageData.width = byteToInt(data);
    elif (name == "height"):
        imageData.height = byteToInt(data);
    elif (name == "image"):
        if ((imageData.width==0) or (imageData.height==0)):
          ReturnError(channel, "Error - missing image dimensions before image data!")
          raise RuntimeError, "Error - missing image dimensions before image data!"
        if (dlen!=(imageData.width*imageData.height*3)):
          ReturnError(channel, "Error - length of data and dimensions of image\n        disagree! (width:"+imageData.width+" height:"+imageData.height+" len:"+len+")\n")
          raise RuntimeError, "Error - length of data and dimensions of image\n        disagree! (width:"+imageData.width+" height:"+imageData.height+" len:"+len+")\n"

        # we did not read in the image data yet since it is always > 1024 ..
        imageData.pixels = buffer(channel.read(dlen))
    else:
        # skip this variable by reading in any remaining data
        if dlen > DATA_BUFFER:
	        channel.read(dlen)
    return 1

######################## Main ########################################

#create our image data object
imageData = ImageDataType()
 
#initialize the server socket  
mySocket = socket.socket ( socket.AF_INET, socket.SOCK_STREAM )
mySocket.bind ( ('', SERVER_PORT ) )
mySocket.listen ( 1 )
while True:
	try:
		rawChannel, details = mySocket.accept()
		print 'We have opened a connection with', details
		channel = MySocket(rawChannel)

		while True:
			imageData.count=imageData.count+1

			# *1* Comment the next line out after debugging is done!!
			print "Processing "+str(imageData.count)

			while True:
				# read in RR variable length
				vlen = byteToInt(channel.read(4))
				# if length <=0 on the variable name then we're done
				if (vlen<=0): break
				
				# read in variable name 
				varName = channel.read(vlen)
				
				# read in the variable's data length
				dlen = byteToInt(channel.read(4))
				
				# but if the name is longer than 64 characters
				# then grab the first 64 chars only
				if (dlen<DATA_BUFFER):
					varData = channel.read(dlen)
				else:
					varData = ""

				# handle this variable
				if (ProcessVariable(channel, imageData, varName, varData, dlen)<0):
					dlen=-1
					break

				# termination signal -1 on attribute length
				if (len==-1): break

			# done collecting variables, now process the image
			ProcessImage(imageData)

			# Write out the processed image back to RoboRealm
			ReturnBytesVariable(channel, "image", newPixels, imageData.width*imageData.height*3)

			# start returning any changed variables

			# send back the count as an example of how to feed back variables into RoboRealm
			ReturnIntVariable(channel, "count", imageData.count);

			# write out end of message
			channel.write(intToByte(0), 4)

			# continue by waiting for next image request

		# opps, we got disconnected
		print "Disconnected."

		channel.close()
	except socket.error, msg:
			# if anything bad happened above, this runs:
			print "An error occurred:", msg
			channel.close()
