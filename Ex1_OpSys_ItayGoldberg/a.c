#include<stdio.h> 
#include<fcntl.h> 
#include<stdlib.h> 
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

int main(int argc, char * argv[])
{
	// define input files descriptors
	int fdin1, fdin2;            
	fdin1 = open(argv[1], O_RDONLY);
	fdin2 = open(argv[2], O_RDONLY);
	
	int readfd1, readfd2;
	char ch1, ch2;
	int result = 0;
	
	do {
	readfd1 = read(fdin1, &ch1, 1);
	readfd2 = read(fdin2, &ch2, 1);
	if(readfd1 == 0 && readfd2 == 0)
		result = 2;
	else
		{
		if(readfd1 == 0 || readfd2 == 0)
			result = 1;
		if(ch1 != ch2)
			result = 1;
		}
	} while(result == 0);
	
	close(fdin1);
	close(fdin2);	
		
	if (result == 2)
		exit(2);
	else
		exit(1);
}
