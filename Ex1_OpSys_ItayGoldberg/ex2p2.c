/*
   	Ex1p2.c
 	Itay Aharon Goldberg
	16.12.22
	Operating Systems 83381-01 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <string.h>

void get_names_from_file(FILE* file, char*** names, int* num_names);
char** read_words_from_file(const char* filename);
void free_words(char** words);

int main(int argc, char * argv[])
{
	int i;
	char ch1;	
	int stat;
	int ret;

		// open grades sheet
	int gradesSheet_fd = open("grades.csv", O_CREAT | O_RDWR , 0666);
	if (gradesSheet_fd == -1) 
	{
  		perror("Error opening grades.csv");
  		exit(1);
	}
	
		// read the config into 3 arg
	FILE* config_file = fopen(argv[1], "r");
  	if (!config_file) 
	{
    	printf("Failed to open file\n");
    	return 1;
	}
	// Declare a variable to store the array of names and a variable to store the number of names in the file
	char** pathtofiles;
	int num_names3;
	// Call the function to get the names from the file
	get_names_from_file(config_file, &pathtofiles, &num_names3);
 	char *dir_r = pathtofiles[0];
    char *in_name_r = pathtofiles[1];
    char *out_name_r = pathtofiles[2];

	char** words = read_words_from_file(in_name_r);
	if (words == NULL) 
	{
    	return 1;
	}

	// Print the words to the console
  	char in_arg[128];
	memset(in_arg, '\0', sizeof(in_arg));
	strcpy(in_arg, words[0]);
	for (i = 1; words[i] != NULL; i++) 
	{
		strcat(in_arg, " ");
		strcat(in_arg, words[i]);
	}
	
	char** outs = read_words_from_file(out_name_r);
  	char out_arg[128];
	memset(out_arg, '\0', sizeof(out_arg));
	strcpy(out_arg, out_name_r);
	char *aaa = "expectedOutput.txt";

	int studentsList_fd = open("studentsList.txt", O_CREAT | O_RDWR, 0666);
	pid_t stdpid = fork();
	if (stdpid == -1)
	{
  		perror("Error");
		exit(1);
	}
	if (stdpid == 0)
	{
		close(1);
		close(2);
		dup(studentsList_fd);
		close(studentsList_fd);
		char* commands[] = {"ls", dir_r, NULL};
		execvp("ls", commands);
		exit(-1);
	}
	else
		wait(NULL);
	close(studentsList_fd);
	waitpid(stdpid, NULL,0);

	// Open the file for reading
	FILE* file = fopen("studentsList.txt", "r");
	if (!file) 
	{
    	printf("Failed to open file\n");
    	return 1;
	}
  	// Declare a variable to store the array of names and a variable to store the number of names in the file
  	char** names;
  	int num_names;
  	// Call the function to get the names from the file
  	get_names_from_file(file, &names, &num_names);

		// 3 part loop thats test all the students and hendle their programs ouyputs
	for (i=0; i<num_names; i++) {		
		//1st part - run the program of the current student
		char path[128];
		memset(path, '\0', sizeof(path));
		strcpy(path, dir_r);
		strcat(path, "/");
		strcat(path, names[i]);
		strcat(path, "/a.out");
				
		pid_t outpid = fork();
		if (outpid == -1)
		{
  			perror("Error");
  			exit(1);
		}
		if (outpid == 0) 
		{
    		// Open output file and set as stdout
			int out_fd = open("out.txt",  O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (out_fd == -1) 
			{
  				perror("Error opening out.txt");
  				exit(1);
			}
			close(1);
			dup(out_fd);
			close(out_fd);
			// execute the process
			char* commands[] = {path, words[0],words[1], NULL};
			exit(execvp(path, commands));
		}
		else
    		wait(&stat);

		//2nd part - cheking the output by compering to expectedOutput.txt by comp.out
		waitpid(outpid,NULL,0);
		pid_t compid = fork();
		if (compid == -1)
		{
  			perror("Error");
  			exit(1);
		}
		if (compid == 0)
		{
			close(1);
			dup(gradesSheet_fd);
			close(gradesSheet_fd);
			char* commands2[] = {"./comp.out", out_arg, "out.txt",NULL};	
			execvp("./comp.out", commands2);
			exit(-1);
		}
		else
			wait(&stat);
		ret = WEXITSTATUS(stat);

		//3rd part - calculate and write each grade to grades.csv	
		waitpid(compid,NULL,0);
		char gradetoprunt[50]="";

		pid_t gspid = fork();
		if(gspid == -1)
		{
			perror("fork faild");
			exit(-1);
		}
		if(gspid == 0)
		{
		// Call comp.out with the appropriate arguments
			waitpid(gspid,NULL,0);
    		char* args3[] = {"./comp.out", " .txt", out_arg, NULL};
    		execv(args3[0],args3);
		}
		else
		{
			//get return value from the comp.out
			waitpid(gspid,&stat,0);
			if (ret == 2){
				strcat(gradetoprunt,names[i]);
				strcat(gradetoprunt, ",100\n");
			}
			else if (ret == 1){
				strcat(gradetoprunt,names[i]);
				strcat(gradetoprunt, ",0\n");
			}
			else
				printf("error\n");
				int writef1 = write(gradesSheet_fd,gradetoprunt,sizeof(gradetoprunt));
				if(writef1 == -1)
			{
				perror("write faild");
				exit(-1);
			}
		}		
	} 	
	// Free the memory allocated for the names
  	for (i = 0; i < num_names; i++) {
    	free(names[i]); }
  	free(names);
  	// Close the file
  	fclose(file);

  	for (i = 0; i < num_names3; i++) {
    	free(pathtofiles[i]); }
  	free(pathtofiles);

	free_words(words);
	free_words(outs);

	// Close the file
	fclose(config_file);

	close(gradesSheet_fd);
	exit(1);
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

// Function to read the file and return an array of strings
// containing the words in the file.
char** read_words_from_file(const char* filename) {
	// Open the file for reading
	FILE* file = fopen(filename, "r");
	if (file == NULL) 
	{
    	fprintf(stderr, "Error: Unable to open file '%s' for reading.\n", filename);
    	return NULL;
	}

	// Allocate memory for an array of pointers to strings
	char** words = malloc(sizeof(char*));
	if (words == NULL) 
	{
    	fprintf(stderr, "Error: Unable to allocate memory for words.\n");
    	return NULL;
	}

	// Initialize the word count and allocate memory for the first word
	int word_count = 0;
	words[word_count] = malloc(sizeof(char));
	if (words[word_count] == NULL) 
	{
    	fprintf(stderr, "Error: Unable to allocate memory for first word.\n");
    	return NULL;
	}

	// Read the file character by character
  	int c, i = 0;
  	while ((c = fgetc(file)) != EOF) 
	  {
    	// If the character is a space, we have reached the end of a word
    	if (c == ' ') 
		{
      		// Terminate the current word and increment the word count
      		words[word_count][i] = '\0';
      		word_count++;

      		// Allocate memory for the next word
      		words[word_count] = malloc(sizeof(char));
      		if (words[word_count] == NULL) 
			{
        		fprintf(stderr, "Error: Unable to allocate memory for next word.\n");
        		return NULL;
      		}

      		// Reset the character index for the next word
      		i = 0;
    	} 
		else 
		{
      		// Append the character to the current word
      		words[word_count][i] = c;
      		i++;

      		// Reallocate memory for the next character
      		words[word_count] = realloc(words[word_count], (i + 1) * sizeof(char));
      		if (words[word_count] == NULL) 
			{
        		fprintf(stderr, "Error: Unable to allocate memory for next character.\n");
        		return NULL;
      		}
    	}
  	}
	// Terminate the last word and close the file
	words[word_count][i] = '\0';
	fclose(file);

	// Return the array of words
	return words;
}

// Function to free the memory used by the array of words
void free_words(char** words) {
	// Free each word in the array
	int i;
	for (i = 0; words[i] != NULL; i++) 
	{
    free(words[i]);
	}

	// Free the array itself
	free(words);
}