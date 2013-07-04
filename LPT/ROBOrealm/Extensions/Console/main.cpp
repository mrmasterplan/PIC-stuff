#include "stdio.h"
#include "stdlib.h"
#include "windows.h"

void logIt(char *txt)
{
	FILE *fp = fopen("c:\\temp\\log.txt", "a+t");
	if (fp)
	{
		fprintf(fp, "%s\n", txt);
		fclose(fp);
	}
}

int main(int argc, char *argv[])
{
	int i;
/*
HANDLE hStdout; 

hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 

unsigned long int dwWritten;
WriteFile(hStdout, "help\n", 5, &dwWritten, NULL); 
*/
	logIt("started");

	fprintf(stderr, "Program Arguments\n");
	for (i=0;i<argc;i++)
	{
		fprintf(stderr, "%s\n", argv[i]);
		logIt(argv[i]);
	}
	fprintf(stderr, "\n");

	char buffer[1024];
	while (true)
	{
	logIt("waiting for input");
		gets(buffer);
	logIt(buffer);

		fprintf(stderr, "GOT %s\n", buffer);

		for (i=0;buffer[i];i++)
			buffer[i]=toupper(buffer[i]);

		printf("%s\n", buffer);
	}

	return 1;
}