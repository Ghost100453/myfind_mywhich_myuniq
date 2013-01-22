#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

// A path string array is created to store the three paths.
char path[2][1000];

// A success variable is initialized to keep track of whether any of the files
// weren't found.
int success=0;

extern void whichfile(char* filename);

int main(int argc, char **argv)
{
    // If the program is passed no arguments then the program exits with a 1 
    // value and a usage message is printed to standard error.
	if (argc == 1) {
    	fprintf(stderr, "usage: %s commandname ...\n", argv[0]);
    	return(1);
    }

    // A character array of size 3, each with 1000 bytes, store the three 
    // potential paths to the
    // file.
    
    // The integers i used in the upcoming for loops is initialized.
    int i;

    // getcwd grabs the current directory path and if it is too long, it 
    // returns an error and the program is closed.
    if (getcwd(path[2], sizeof(path[2])) == NULL) {
    	fprintf(stderr, "The directory path is too long.\n");
    	return(1);
    }

    strcpy(path[0], "/bin/");
    strcpy(path[1], "/bin/usr/");
    strcat(path[2], "/");

    // For each of the arguments passed to the program, it is checked for 
    // existance and owner execution in the appropriate order. If it is found 
    // and execuable, then the path is printed to the terminal and the success
    // integer is set to 1. If when all the checks have finished,
    // success is set to 0 and no file has been found, then the program prints
    // a message to indicate that the command wasn't found.
    for (i=1; i < argc; i++) {
        whichfile(argv[i]);
    }
    // The program has executed successfully and exits with a 0.
    return(success);
}

void whichfile(char* filename) 
{
    // A stat struct is initialized so that files can be checked for existance 
    // and execution by the owner.
    struct stat sb;

    // the integer j is initialized to loop over the three potential paths.
    int j;

    for (j = 0; j <= 2; j++) {
        // The file is checked if it exceeds the string size limit when combined
        // with the path and if it does, a message is printed to standard error
        // and the program continues with any other files.
        // Success is set to 1 to mark that there was an error.
        if (strlen(filename) + strlen(path[j]) > 1000) {
            fprintf(stderr, "The path/filename exceeds the length limit.\n");
            success = 1;
            return;
        }

        // The path a filename are combined into one string.
        strcat(path[j], filename);

        // The file is checked for existence, execution and that it is not a 
        // directory.
        if ((stat(path[j], &sb) >= 0) && (sb.st_mode > 0) && 
            (S_IEXEC & sb.st_mode) && !(sb.st_mode & S_IFDIR))  {
                printf("%s\n", path[j]);
                path[j][strlen(path[j]) - strlen(filename)] = '\0';
                return;
        }

        // The filename is removed form the previously created [path]/filename 
        // string.
        path[j][strlen(path[j]) - strlen(filename)] = '\0';

    }

    // If this line was reached, no errors occured and executable file doesn't 
    // exist in those three directories.
    fprintf(stderr, "%s: Command not found.\n", filename);
    success = 1;
    return;
}