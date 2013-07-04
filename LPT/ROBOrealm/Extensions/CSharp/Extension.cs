using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Text;

namespace Extension
{
    /////////////////////////////// Data Structures ///////////////////////////////

    // hold the image data received from RR
    class ImageDataType
    {
      // hold the name used in RR to identify this module
      public String name;
      // image processed count ... sent back to RR
      public int count;
      // dimensions of the received image
      public int width, height;
      // holds the image data
      public byte[] pixels = null;
      // holds image data length to test if image size changes and buffer becomes too small
      public int allocLen;
    };

    public class Handler
    {
	    // buffer size used when reading in variable data
	    public const int DATA_BUFFER = 1024;

	    // the maximum length of a variable name to read
	    public const int MAX_VARNAME_SIZE = 64;

	    // the port number to listen on ... needs to match that used in RR interface
	    public const int SERVER_PORTNUM = 5050;

        // the number of milliseconds a read should wait before timing out
        public const int TIMEOUT = 5000;

        // buffer returned by intToByte
	    private byte[] intToByteBuffer = new byte[4];

        // Volatile is used as hint to the compiler that this data
        // member will be accessed by multiple threads.
        private volatile bool isRunning = true;

        /////////////////////////////// Image processing //////////////////////////////

	    // Performs the image conversion/processing/etc that you want to perform
	    void ProcessImage(ImageDataType imageData)
	    {
		    byte[] pixels = imageData.pixels;
		    int width = imageData.width;
		    int height = imageData.height;
		    int i;
		    // remember that we're working in BGR color ... so the actual width is 3x
		    int w=width*3;
		    byte tmp;

		    // sanity check
		    if ((pixels==null)||(width==0)||(height==0)) return;

		    // you could use imageData.getName() to perform different analysis in the same
		    // program ....

		    // as an example ... swap red and blue values
		    for (i=0;i<w*height;i+=3)
		    {
			    // normal image format is BGR ... first byte is blue
			    tmp = pixels[i];
			    // swap red and blue
			    pixels[i] = pixels[i+2];

			    pixels[i+2] = tmp;
		    }

		    // note this this is an inplace filter so you don't need an additional image array
		    // but you've now corrupted the original image ...
	    }

	    /////////////////////////////// Data Handling /////////////////////////////////

	    int byteToInt(byte[] data)
	    {
		    return (data[0]&255)|((data[1]&255)<<8)|((data[2]&255)<<16)|((data[3]&255)<<24);
	    }

	    byte []intToByte(int num)
	    {
		    intToByteBuffer[0] = (byte)(num&255);
		    intToByteBuffer[1] = (byte)((num>>8)&255);
		    intToByteBuffer[2] = (byte)((num>>16)&255);
		    intToByteBuffer[3] = (byte)((num>>24)&255);
		    return intToByteBuffer;
	    }

	    // returns an error message to RR. This message is displayed in the "messages" list within
	    // the RR Pipe Program interface.
	    void ReturnError(BinaryWriter writer, string txt)
	    {
		    writer.Write(intToByte(5));
            writer.Write(Encoding.ASCII.GetBytes("error"));
		    writer.Write(intToByte(txt.Length));
            writer.Write(Encoding.ASCII.GetBytes(txt));
	    }

	    // returns a byte string variable to RR. The returned variables can be used
	    // elsewhere in RR for continued processing.
	    void ReturnBytesVariable(BinaryWriter writer, string name, byte[] data, int len)
	    {
		    writer.Write(intToByte(name.Length));
            writer.Write(Encoding.ASCII.GetBytes(name));

		    writer.Write(intToByte(len));
		    writer.Write(data, 0, len);
	    }

	    // returns an int variable to RR
	    void ReturnIntVariable(BinaryWriter writer, string name, int num)
	    {
		    string strTmp = num.ToString();

		    writer.Write(intToByte(name.Length));
            writer.Write(Encoding.ASCII.GetBytes(name));

		    writer.Write(intToByte(strTmp.Length));
            writer.Write(Encoding.ASCII.GetBytes(strTmp));
	    }

	    /////////////////////////////// Data Processing ///////////////////////////////

	    int readBytes(byte[] data, BinaryReader reader, int len)
	    {
		    int res;
		    int index=0;

		    while (len>0)
		    {
			    if ((res=reader.Read(data, index, len))<0)
				    return -1;

			    index+=res;
			    len-=res;
		    }
		    return index;
	    }

	    // Parses the variables sent by RR into the appropriate structure. You can add
	    // your own processing routines here to handle other variables that may get sent.
	    int ProcessVariable(BinaryWriter writer, BinaryReader reader, ImageDataType imageData, string name, byte[] data, int len)
	    {
            name = name.ToLower();

            // determine what we've got
		    if (name.Equals("name"))
		    {
			    imageData.name = name;
		    }
		    else
		    // determine what we've got
		    if (name.Equals("width"))
		    {
			    imageData.width = byteToInt(data);
		    }
		    else
		    if (name.Equals("height"))
		    {
			    imageData.height = byteToInt(data);
		    }
		    else
		    if (name.Equals("image"))
		    {
			    if ((imageData.width==0)||(imageData.height==0))
			    {
				    ReturnError(writer, "Error - missing image dimensions before image data!");
				    Console.WriteLine("Error - missing image dimensions before image data!");
				    return -1;
			    }

			    if (len!=(imageData.width*imageData.height*3))
			    {
				    ReturnError(writer, "Error - length of data and dimensions of image\n        disagree! (width:"+imageData.width+" height:"+imageData.height+" len:"+len+")\n");
				    Console.WriteLine("Error - length of data and dimensions of image\n        disagree! (width:"+imageData.width+" height:"+imageData.height+" len:"+len+")\n");
				    return -1;
			    }

			    // we only need to allocate once! The program will remain
			    // active for as long as processing continues ...
			    if (imageData.pixels==null)
			    {
				    imageData.pixels = new byte[len];
				    imageData.allocLen = len;
			    }
			    else
			    {
				    // but we need to check to see if the image size has changed that we have
				    // enough room to load it in
				    if (imageData.allocLen<len)
				    {
					    imageData.pixels = new byte[len];
					    imageData.allocLen = len;
				    }
			    }

			    // we did not read in the image data yet since it is always > 1024 ..
			    int res;
			    if ((res=readBytes(imageData.pixels, reader, len))!=len)
			    {
				    Console.WriteLine("Error - read failed. Wanted "+len+" bytes but got "+res+"\n");
				    return -1;
			    }
		    }
		    else
		    {
			    // skip this variable
			    if (len>DATA_BUFFER)
			    {
                    while (len > 0)
                    {
                        reader.Read();
                        len--;
                    }
			    }
		    }

		    return 1;
	    }

    	/////////////////////////////// Main //////////////////////////////////////////
        // This method will be called when the thread is started.
        public void run()
        {
		    // holds the variable name being sent by RR
		    string varName;
		    // holds the received and prehaps processed image data
		    byte[] varData = new byte[DATA_BUFFER];
		    // byte array for incoming integer number
		    byte[] number = new byte[4];

            IPAddress myIp = IPAddress.Parse("127.0.0.1");
            IPEndPoint ipEnd = new IPEndPoint(myIp, SERVER_PORTNUM);

            Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            server.Bind(ipEnd);

		    Console.WriteLine("RoboRealm Java Socket Server v0.0.1 Listening On Port "+SERVER_PORTNUM+"....\n");

     		ImageDataType imageData = new ImageDataType();
            
            while (isRunning)
            {
				Console.WriteLine("Waiting ...\n");

                server.Listen(10);

				Socket client = server.Accept();

                Console.WriteLine("Connected.\n");

                client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, TIMEOUT);
                client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, TIMEOUT);

                NetworkStream networkStream = new NetworkStream(client);
                BinaryWriter binaryWriter = new BinaryWriter(networkStream);
                BinaryReader binaryReader = new BinaryReader(networkStream);

				imageData.count=0;

				int len=0;

				while (true)
				{
					imageData.count++;

					// *1* Comment the next line out after debugging is done!!
					Console.WriteLine("Processing "+imageData.count+"\r");

		            try
                    {
					    while (true)
					    {
						    // read in variable length
						    binaryReader.Read(number, 0, 4);
						    len = byteToInt(number);
						    // if length <=0 on the variable name then we're done
						    if (len<=0) break;
						    // read in variable name but if the name is longer than 64 characters
						    // then grab the first 64 chars only
						    byte[] varNameChar = new byte[len];
						    binaryReader.Read(varNameChar, 0, len);
						    varName = System.Text.Encoding.ASCII.GetString(varNameChar);

						    // read in the variable's data length
						    binaryReader.Read(number, 0, 4);
						    len = byteToInt(number);
						    // if the data is less than 1024 read it in now ..
						    if (len<DATA_BUFFER)
						    {
							    binaryReader.Read(varData, 0, len);
						    }

						    // handle this variable
						    if (ProcessVariable(binaryWriter, binaryReader, imageData, varName, varData, len)<0)
						    {
							    len=-1;
							    break;
						    }
					    }

					    //Done collecting variables.

					    // termination signal -1 on attribute length
					    if (len==-1) break;

					    // process image
					    ProcessImage(imageData);

					    //Returning variables.

					    // Write out the processed image back to RoboRealm using stdout.
					    // You can also write back any other variables to use in
					    // other parts of the program.
					    // The format is the same as the input.
					    ReturnBytesVariable(binaryWriter, "image", imageData.pixels, imageData.width*imageData.height*3);

					    //Returned image;

					    // Send back the count as an example of how to feed back variables into RoboRealm
					    ReturnIntVariable(binaryWriter, "count", imageData.count);

					    // write out end of message
					    binaryWriter.Write(intToByte(0));

					    // flush all data back to RR
					    binaryWriter.Flush();
					    // continue by waiting for next image request
				    }
                    catch (Exception e)
                    {
        				//ignore any timeout errors by closing connection and restarting
                        //Console.WriteLine("Caught exception " + e.ToString() + "\n" + e.StackTrace);
                        break;
                    };
                }
				Console.WriteLine("\nDisconnected.\n");

				binaryReader.Close();
				binaryWriter.Close();
				client.Close();

				if (len==-1) break;
            }
        }
        public void RequestStop()
        {
            isRunning = false;
        }
    }

    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            //Application.EnableVisualStyles();
            //Application.SetCompatibleTextRenderingDefault(false);
            //Application.Run(new Form1());

		    if (args.Length>1)
		    {
			    Console.WriteLine("Started with \"");
			    int i;
			    for (i=1;i<args.Length;i++)
			    {
				    if (i>1) Console.WriteLine(" ");
				    Console.WriteLine(args[i]);
			    }
			    Console.WriteLine("\"\n");
		    }
		    else
                Console.WriteLine("Started.\n");

            Handler workerObject = new Handler();
            Thread workerThread = new Thread(workerObject.run);

            // Start the worker thread.
            workerThread.Start();

            // Loop until worker thread activates.
            while (!workerThread.IsAlive);

            // Use the Join method to block the current thread 
            // until the object's thread terminates.
            workerThread.Join();
        }
    }
}