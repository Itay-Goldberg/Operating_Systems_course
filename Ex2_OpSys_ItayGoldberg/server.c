/*
   	Ex2
    server.c
 	Itay Aharon Goldberg
	02.01.23
	Operating Systems 83381-01 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <string.h>

void get_names_from_file(FILE* file, char*** names, int* num_names);

void handler(int sig)
{
    	// read toServer.txt into 4 arg
	FILE* toServer_file = fopen("toServer.txt", "r");
  	if (!toServer_file) 
	{
    	printf("Failed to open file\n");
	}
	// Declare a variable to store the array of names and a variable to store the number of names in the file
	char** clientInfo;
	int clientArgNum;
	// Call the function to get the names from the file
	get_names_from_file(toServer_file, &clientInfo, &clientArgNum);
 	char* clientPid = clientInfo[0];
 	int toCalc_1 = atoi(clientInfo[1]);
    int arithmOperatiom = atoi(clientInfo[2]);
 	int toCalc_2 = atoi(clientInfo[3]);

	pid_t rm_pid = fork();
	if (rm_pid == -1)
	{
  		perror("Error");
		exit(1);
	}
	if (rm_pid == 0)
	{
		char* commands[] = {"rm", "toServer.txt", NULL};
		exit(execvp("rm", commands));
	}
	else
		wait(NULL);

	pid_t calc_handler_pid = fork();
	if (calc_handler_pid == -1)
	{
  		perror("Error");
		exit(1);
	}
	if (calc_handler_pid == 0)
	{
        //write the result to the client
    int result;
    switch(arithmOperatiom)
    {
        case 1:
            result = toCalc_1 + toCalc_2;
            break;
        case 2:
            result = toCalc_1 - toCalc_2;
            break;
        case 3:
            result = toCalc_1 * toCalc_2;
            break;
        case 4:
            result = toCalc_1 / toCalc_2;
            break;
        default:
            result = 0;
            break;
    }
		// open toClient file
    char toClientFile[30];
	memset(toClientFile, '\0', sizeof(toClientFile));
	strcpy(toClientFile, "toClientNum_");	
	strcat(toClientFile, clientPid);
	strcat(toClientFile, ".txt");

	int toClient_fd = open(toClientFile, O_CREAT | O_RDWR , 0666);
	if (toClient_fd == -1) 
	{
  		perror("Error");
  		exit(1);
	}
	pid_t filepid = fork();
	if (filepid == -1)
	{
  		perror("Error");
		exit(1);
	}
	if (filepid == 0)
	{
		close(1);
		dup(toClient_fd);
        printf("%d", result);
		exit(-1);
	}
	else
		wait(NULL);
    close(toClient_fd);

    kill(atoi(clientPid),2);
	exit(0);
	}
	else
		wait(NULL);

    signal(1,handler);
}

int main(int argc, char * argv[]) {

    signal(1,handler);
    while(1)
        pause();

    return 0;
}

void get_names_from_file(FILE* file, char*** names, int* num_names) {
	// First, count the number of names in the file
	*num_names = 0;
	char buffer[1024];
	while (fgets(buffer, sizeof(buffer), file)) {
    	(*num_names)++; }

	// Allocate memory for the array of names
	*names = malloc(*num_names * sizeof(char*));

	// Rewind the file to the beginning
	rewind(file);

	// Read the file again and store the names in the array
	int i = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
    	// Remove the newline character at the end of the line
    	buffer[strcspn(buffer, "\n")] = 0;

    	// Allocate memory for the name and copy it to the array
    	(*names)[i] = strdup(buffer);
    	i++;
  	}
}