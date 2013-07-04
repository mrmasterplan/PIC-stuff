#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

// buffer size used when reading in variable data
#define DATA_BUFFER 1024

// the maximum length of a variable name to read
#define MAX_VARNAME_SIZE 64

// the port number to listen on ... needs to match that used in RR interface
#define SERVER_PORTNUM 5050

WSADATA winsock_data;
WORD version_required; /* Version 1.1 */

/////////////////////////////// Data Structures ///////////////////////////////

// hold the image data received from RR
struct ImageDataType
{
  // hold the name used in RR to identify this module
  char name[16];
  // image processed count ... sent back to RR
  int count;
  // dimensions of the received image
  int width, height;
  // holds the image data
  unsigned char *pixels;
};

/////////////////////////////// Image processing //////////////////////////////

// Performs the image conversion/processing/etc that you want to perform
void ProcessImage(struct ImageDataType *imageData)
{
  unsigned char *pixels = imageData->pixels;
  int width = imageData->width;
  int height = imageData->height;
  int i;
  // remember that we're working in RGB color ... so the actual width is 3x
  int w=width*3;
  int tmp;

  // sanity check
  if ((pixels==NULL)||(width==0)||(height==0)) return;

  // you could use imageData->name to perform different analysis in the same
  // program ....

  // as an example ... swap red and blue values
  for (i=0;i<w*height;i+=3)
  {
    // image format is RGB ... first byte is red
    tmp = pixels[i];
    // swap red and blue
    pixels[i] = pixels[i+2];

    pixels[i+2] = tmp;
  }

  // note this this is an inplace filter so you don't need an additional image array
  // but you've now corrupted the original image ...
}

/////////////////////////////// Data Handling /////////////////////////////////

// returns an error message to RR. This message is displayed in the "messages" list within
// the RR Pipe Program interface.
void ReturnError(int hSocket, char *txt)
{
  int len = 5;
  send(hSocket, (char *)&len, sizeof(int), NULL);
  send(hSocket, "error", 5, NULL);
  len = strlen(txt);
  send(hSocket, (char *)&len, sizeof(int), NULL);
  send(hSocket, txt, len, NULL);
}

// returns a byte string variable to RR. The returned variables can be used
// elsewhere in RR for continued processing.
int ReturnBytesVariable(int hSocket, char *name, unsigned char *data, int len)
{
  int l = strlen(name);
  send(hSocket, (char *)&l, sizeof(int), NULL);
  send(hSocket, name, l, NULL);

  send(hSocket, (char *)&len, sizeof(int), NULL);
  return send(hSocket, (char *)data, len, NULL);
}

// returns an int variable to RR
int ReturnIntVariable(int hSocket, char *name, int num)
{
  char strTmp[32];
  sprintf(strTmp, "%d", num);

  int l = 5;
  send(hSocket, (char *)&l, sizeof(int), NULL);
  send(hSocket, name, strlen(name), NULL);

  int len = strlen(strTmp);
  send(hSocket, (char *)&len, sizeof(int), NULL);
  return send(hSocket, strTmp, len, NULL);
}

int readSocket(int hSocket, unsigned char *buffer, int len)
{
  int num=0;
  do
  {
    int res = recv(hSocket, (char *)&buffer[num], len, NULL);
    if (res<0) return -1;
    num+=res;
    len-=res;
  }
  while (len>0);

  return num;
}

/////////////////////////////// Data Processing ///////////////////////////////

// Parses the variables sent by RR into the appropriate structure. You can add
// your own processing routines here to handle other variables that may get sent.
int ProcessVariable(int hSocket, struct ImageDataType *imageData, char *name, char *data, int len)
{
  // determine what we've got
  if (stricmp(name, "name")==0)
  {
    strncpy(imageData->name, data, 16);
    imageData->name[15]=0;
  }
  else
  // determine what we've got
  if (stricmp(name, "width")==0)
  {
    imageData->width = *(int *)data;
  }
  else
  if (stricmp(name, "height")==0)
  {
    imageData->height = *(int *)data;
  }
  else
  if (stricmp(name, "image")==0)
  {
    if ((imageData->width==0)||(imageData->height==0))
    {
      ReturnError(hSocket, "Error - missing image dimensions before image data!");
      return -1;
    }

    if (len!=(imageData->width*imageData->height*3))
    {
      char buffer[256];
      _snprintf(buffer, 256, "Error - length of data and dimensions of image\n        disagree! (width:%d height%d len:%d)\n", imageData->width, imageData->height, len);
      ReturnError(hSocket, buffer);
      return -1;
    }

    // we only need to allocate once! The program will remain
    // active for as long as processing continues ...
    if (imageData->pixels==NULL)
    {
      if ((imageData->pixels=(unsigned char *)malloc(len))==NULL)
      {
        ReturnError(hSocket, "Error - out of memory.\n");
        exit(0);
      }
    }

    // we did not read in the image data yet since it is always > 1024 ..
    if (readSocket(hSocket, (unsigned char *)imageData->pixels, len)!=len)
      return -1;
  }
  else
  {
    // skip this variable
    if (len>DATA_BUFFER)
    {
      char buffer[1024];
      // only need to skip bytes if we have not read it in yet
      int num = len;
      while (num>0)
      {
        int bytes = recv(hSocket, buffer, num, NULL);
        if (!bytes) break;
        num-=bytes;
      }
    }
  }

  return 1;
}

/////////////////////////////// Main //////////////////////////////////////////

// This is where the program first starts
void main(int argc, char *argv[])
{
  // holds the variable name being sent by RR
  char varName[MAX_VARNAME_SIZE];
  // holds the received and prehaps processed image data
  char varData[DATA_BUFFER];
  // used to hold return value of number of bytes read from pipe
  long bytes;
  // variables data length
  int varLen;

  struct ImageDataType imageData;

  memset(&imageData, 0, sizeof(imageData));

  if (argc>1)
  {
    printf("Started with \"");
    int i;
    for (i=1;i<argc;i++)
    {
      if (i>1) printf(" ");
      printf("%s", argv[i]);
    }
    printf("\"\n");
  }
  else
    printf("Started.\n");

  version_required = 0x0101; /* Version 1.1 */
  WSAStartup (version_required, &winsock_data);

  int hSocket;
  int server;
  int sockaddr_in_length = sizeof(struct sockaddr_in);
  int fromlen = sizeof(struct sockaddr_in);

  if ((server = socket(AF_INET, SOCK_STREAM, 0))<0)
  {
	  printf("Could not create socket!");
    exit(-1);
  }

  int enable=1;

  if ((setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char *)&enable, sizeof(enable)))<0)
  {
	  printf("Could not set socket option SO_REUSEADDR!");
    exit(-1);
  }

  if ((setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, (char *)&enable, sizeof(enable)))<0)
  {
	  printf("Could not set socket option SO_KEEPALIVE!");
    exit(-1);
  }

  if ((setsockopt(server, IPPROTO_TCP, TCP_NODELAY, (char *)&enable, sizeof(enable)))<0)
  {
	  printf("Could not set socket option TCP_NODELAY!");
    exit(-1);
  }

  struct linger ling;
  ling.l_onoff=1;
  ling.l_linger=10;
  if ((setsockopt(server, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(linger)))<0)
  {
	  printf("Could not set socket option SO_LINGER!");
    exit(-1);
  }

  /* sytem's location of the sock */
  struct sockaddr_in sockaddr;
  /* id of foreign calling process */
  struct sockaddr_in caller;
  /* register the sock */
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY; /* not choosy about who calls*/
  sockaddr.sin_port = htons(SERVER_PORTNUM);

  if (bind(server, (struct sockaddr *)&sockaddr, sizeof(sockaddr))!=0)
  {
    exit(-2);
  }

  if (listen(server, 1024)!=0)
  {
    exit(-3);
  }

  while (true)
  {
    printf("Waiting ...\n");

    ::listen(server, 1024);

    if ((hSocket = accept(server, (struct sockaddr *)&caller, &fromlen))<0)
    {
      printf("Accept failed!");
      continue;
    }

    printf("Connected.\n");

    imageData.count=0;

    int len=0;

    while (true)
    {
      imageData.count++;

      printf("Processing %d\r", imageData.count);

      while (true)
      {
        // read in variable length
        bytes = recv(hSocket, (char *)&len, sizeof(len), NULL);
        // if length <=0 on the variable name then we're done
        if ((bytes<=0)||(len<=0)) break;
        // read in variable name but if the name is longer than 64 characters
        // then grab the first 64 chars only
        if (len<MAX_VARNAME_SIZE)
        {
          bytes = recv(hSocket, varName, len, NULL);
        }
        else
        {
          ReturnError(hSocket, "Variable name too long!");
          len=-1;
          break;
        }
        // don't forget to terminate this string with a zero
        varName[bytes]=0;

        // read in the variable's data length
        recv(hSocket, (char *)&varLen, sizeof(varLen), NULL);
        // if the data is less than 1024 read it in now ..
        if (varLen<sizeof(varData))
        {
          recv(hSocket, varData, varLen, NULL);
        }

        // handle this variable
        if (ProcessVariable(hSocket, &imageData, varName, varData, varLen)<0)
        {
          bytes=-1;
          break;
        }
      }

      // termination signal -1 on attribute length
      if ((len==-1)||(bytes<=0)) break;

      // process image
      ProcessImage(&imageData);

      // Write out the processed image back to RoboRealm using stdout.
      // You can also write back any other variables to use in
      // other parts of the program.
      // The format is the same as the input.
      ReturnBytesVariable(hSocket, "image", imageData.pixels, imageData.width*imageData.height*3);

      // Send back the count as an example of how to feed back variables into RoboRealm
      ReturnIntVariable(hSocket, "count", imageData.count);

      // write out end of message
      len = 0;
      send(hSocket, (char *)&len, sizeof(int), NULL);
      if (bytes==0) break;

      // continue by waiting for next image request
    }

    printf("\nDisconnected.\n");

    shutdown(hSocket, 1);
    shutdown(hSocket, 0);
    closesocket(hSocket);

    if (len==-1) break;
  }

  printf("Exiting\n");

  // free alloced array now that we're done
  free(imageData.pixels);

  closesocket(server);
}
