#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// size of the pipe buffer used to communicate to/from RR
#define PIPE_BUFFER_SIZE 4096

// buffer size used when reading in variable data
#define DATA_BUFFER 1024

// the maximum length of a variable name to read
#define MAX_VARNAME_SIZE 64

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

// returns an error message to RR. This message is displayed in the "messages" list within
// the RR Pipe Program interface.
void ReturnError(HANDLE hPipe, char *txt)
{
  unsigned long bytes;
  int len = 5;
  WriteFile(hPipe, &len, sizeof(int), &bytes, NULL);
  WriteFile(hPipe, "error", 5, &bytes, NULL);
  len = strlen(txt);
  WriteFile(hPipe, &len, sizeof(int), &bytes, NULL);
  WriteFile(hPipe, txt, len, &bytes, NULL);
}

// returns a byte string variable to RR. The returned variables can be used
// elsewhere in RR for continued processing.
int ReturnBytesVariable(HANDLE hPipe, char *name, unsigned char *data, int len)
{
  unsigned long bytes;
  int l = strlen(name);
  WriteFile(hPipe, &l, sizeof(int), &bytes, NULL);
  WriteFile(hPipe, name, l, &bytes, NULL);

  WriteFile(hPipe, &len, sizeof(int), &bytes, NULL);
  WriteFile(hPipe, data, len, &bytes, NULL);

  return bytes;
}

// returns an int variable to RR
int ReturnIntVariable(HANDLE hPipe, char *name, int num)
{
  char strTmp[32];
  sprintf(strTmp, "%d", num);

  unsigned long bytes;
  int l = 5;
  WriteFile(hPipe, &l, sizeof(int), &bytes, NULL);
  WriteFile(hPipe, name, strlen(name), &bytes, NULL);

  int len = strlen(strTmp);
  WriteFile(hPipe, &len, sizeof(int), &bytes, NULL);
  WriteFile(hPipe, strTmp, len, &bytes, NULL);

  return bytes;
}

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

// Parses the variables sent by RR into the appropriate structure. You can add
// your own processing routines here to handle other variables that may get sent.
void ProcessVariable(HANDLE hPipe, struct ImageDataType *imageData, char *name, char *data, int len)
{
  unsigned long bytes;

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
      ReturnError(hPipe, "Error - missing image dimensions before image data!");
      exit(0);
    }

    if (len!=(imageData->width*imageData->height*3))
    {
      char buffer[256];
      _snprintf(buffer, 256, "Error - length of data and dimensions of image\n        disagree! (width:%d height:%d len:%d)\n", imageData->width, imageData->height, len);
      ReturnError(hPipe, buffer);
      exit(0);
    }

    // we only need to allocate once! The program will remain
    // active for as long as processing continues ...
    if (imageData->pixels==NULL)
    {
      if ((imageData->pixels=(unsigned char *)malloc(len))==NULL)
      {
        ReturnError(hPipe, "Error - out of memory.\n");
        exit(0);
      }
    }

    // we did not read in the image data yet since it is always > 1024 ..
    ReadFile(hPipe, imageData->pixels, len, &bytes, NULL);
  }
  else
  {
    // skip this variable
    if (len>DATA_BUFFER)
    {
      // only need to skip bytes if we have not read it in yet
      SetFilePointer(hPipe, len, 0, FILE_CURRENT);
    }
  }
}

// This is where the program first starts
void main(int argc, char *argv[])
{
  // the pipe name to use ... default is rrpipe but can be changed in RR interface
  LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\rrpipe");
  // holds the variable name being sent by RR
  char varName[MAX_VARNAME_SIZE];
  // holds the received and prehaps processed image data
  char varData[DATA_BUFFER];
  // used to hold return value of number of bytes read from pipe
  unsigned long bytes;
  // variables data length
  int varLen;

  struct ImageDataType imageData;

  memset(&imageData, 0, sizeof(imageData));

  HANDLE hPipe;

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


  // Create the pipe used to communicate with RR
  hPipe = CreateNamedPipe(
          lpszPipename,             // pipe name
          PIPE_ACCESS_DUPLEX,       // read/write access
          PIPE_TYPE_MESSAGE |       // message type pipe
          PIPE_READMODE_MESSAGE |   // message-read mode
          PIPE_WAIT,                // blocking mode
          PIPE_UNLIMITED_INSTANCES, // max. instances
          PIPE_BUFFER_SIZE,         // output buffer size
          PIPE_BUFFER_SIZE,         // input buffer size
          NMPWAIT_USE_DEFAULT_WAIT, // client time-out
          NULL);                    // default security attribute

  if (hPipe == INVALID_HANDLE_VALUE)
  {
    printf("CreatePipe failed");
    exit(0);
  }

  while (true)
  {
    printf("Waiting ...\n");

    if (!ConnectNamedPipe(hPipe, NULL)? TRUE:(GetLastError() == ERROR_PIPE_CONNECTED))
    {
      printf("ConnectNamedPipe failed");
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
        ReadFile(hPipe, &len, sizeof(len), &bytes, NULL);
        // if length <=0 on the variable name then we're done
        if (len<=0) break;
        // read in variable name but if the name is longer than 64 characters
        // then grab the first 64 chars only
        if (len<MAX_VARNAME_SIZE)
        {
          ReadFile(hPipe, varName, len, &bytes, NULL);
        }
        else
        {
          // read in first 64 chars
          ReadFile(hPipe, varName, MAX_VARNAME_SIZE-1, &bytes, NULL);
          // skip over remaining chars
          SetFilePointer(hPipe, len-MAX_VARNAME_SIZE+1, 0, FILE_CURRENT);
        }
        // don't forget to terminate this string with a zero
        varName[bytes]=0;

        // read in the variable's data length
        ReadFile(hPipe, &varLen, sizeof(varLen), &bytes, NULL);
        // if the data is less than 1024 read it in now ..
        if (varLen<sizeof(varData))
        {
          ReadFile(hPipe, varData, varLen, &bytes, NULL);
        }

        // handle this variable
        ProcessVariable(hPipe, &imageData, varName, varData, varLen);
      }

      // termination signal -1 on attribute length
      if (len==-1) break;

      // process image
      ProcessImage(&imageData);

      // Write out the processed image back to RoboRealm using stdout.
      // You can also write back any other variables to use in
      // other parts of the program.
      // The format is the same as the input.
      ReturnBytesVariable(hPipe, "image", imageData.pixels, imageData.width*imageData.height*3);

      // Send back the count as an example of how to feed back variables into RoboRealm
      ReturnIntVariable(hPipe, "count", imageData.count);

      // write out end of message
      len = 0;
      WriteFile(hPipe, &len, sizeof(int), &bytes, NULL);
      if (bytes==0) break;

      // continue by waiting for next image request
    }

    printf("\nDisconnected.\n");

    DisconnectNamedPipe(hPipe);

    if (len==-1) break;
  }

  printf("Exiting\n");

  // free alloced array now that we're done
  free(imageData.pixels);

  CloseHandle(hPipe);
}