/*
   	Ex2
    client.c
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

#define MAX_TRIES 10

void handler(int sig)
{   
    	// open toClient file
    int clientPidInt = getpid(); // clientPid <- PID
    //printf("debug - signal 2 was received from the server to client %d\n", clientPidInt);
    char clientPid[6]; 
    sprintf(clientPid, "%d", clientPidInt);

    char toClientFile[30];
	memset(toClientFile, '\0', sizeof(toClientFile));
	strcpy(toClientFile, "toClientNum_");	
	strcat(toClientFile, clientPid);
	strcat(toClientFile, ".txt");

    char buf[10];
    int toClient_fd = open(toClientFile, O_RDWR);
    int dignum = read(toClient_fd,buf,10);
    int toPlot = atoi(buf);
    printf("Client (PID = %d): The calculation result received from the server is %d\n",clientPidInt, toPlot);

	pid_t rm_pid = fork();
	if (rm_pid == -1)
	{
  		perror("Error");
		exit(1);
	}
	if (rm_pid == 0)
	{
		char* commands[] = {"rm", toClientFile, NULL};
		exit(execvp("rm", commands));
	}
	else
		wait(NULL);

    signal(2,handler);
}

int main(int argc, char * argv[]) {

    int i; //index
    pid_t clientPid = getpid(); // clientPid <- PID
    int serverPid = atoi(argv[1]);

    int num_tries = 0;
    while (num_tries < MAX_TRIES)
    {
        if (access("toServer.txt", F_OK) == 0)
        {
            // File exists
            printf("File in use, trying again...\n");
            sleep((rand() % 10) + 1); // Sleep for a random number of seconds between 1 and 10
            num_tries++;
        }
        else
            break; 
    }
    if (num_tries == MAX_TRIES)
    {
        printf("Not able to talk with the server\n ");
        return 0;
    }

    	// open file and write pros_id & argv[] into it
	int toServer_fd = open("toServer.txt", O_CREAT | O_RDWR , 0666);
	pid_t filepid = fork();
	if (filepid == -1)
	{
  		perror("Error");
		exit(1);
	}
	if (filepid == 0)
	{
		close(1);
		dup(toServer_fd);
        printf("%d\n", clientPid);
        for (i=2;i<argc;i++)
        {
            printf("%s\n", argv[i]);
        }
		exit(-1);
	}
	else
		wait(NULL);
    close(toServer_fd);

        //sending signal to the server
    //printf("debug - signal 1 was sended from client %d to server %d\n",clientPid, serverPid);
    kill(serverPid,1);
    signal(2,handler);
    pause();

    return 0;
}
