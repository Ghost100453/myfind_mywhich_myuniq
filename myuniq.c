#include <stdio.h>
#include <string.h>
#include <unistd.h>

// The c argument is set to 0 by default, the option integer is inititalized.
// The sucess variable is set to 0 by default.
int c = 0, success = 0, opt = 0;

extern void noline (char *s);

int main(int argc, char **argv)
{
    // The arguments are fed into getopt and the user is warned if any options
    // other than 'c' are entered as well as setting success to 1.
    while ((opt = getopt(argc,argv,":c")) != -1) {
        switch (opt) {
            case 'c':
                c=1;
                break;
            default:
                fprintf(stderr,"%s: invalid option -- '%c'\n",argv[0], 
                    (char)optopt);               
                success=1;
                break;
        }
    }

    // If myuniq is executed with arguments, success is set to 1.
    if (optind >= argc) {
        success=1;
    }

    // A pointer to a file is initialized to open and read files.
    FILE *fr;

    // If incorrect options were given or no filenames were given then
    // success == 1, in which case the program exits with a status of 1.
    // When the program exits, a proper usage message is sent.
    if (success) {
        fprintf(stderr,"Usage: %s [-c] [file ...]\n", argv[0]);
        return(success);
    }

    // The integer reps, which keeps track of how many times a line is repeated,
    // is initialized as well as the two character arrays for each line 
    // (500 byes each).
    int reps=0;
    char line1[500];
    char line2[500];

    // This for loop will loop through all the arguments passed to the program
    // which were not options.
    for(; optind < argc; optind++) {

        // fopen is used to attempt to open the file. If that opening should 
        // fail, then success is set to 1 and the file is skipped over. 
        // Otherwise, the file is opened. Upon failure, a perror with the file 
        // name is also called.
        if ((fr = fopen (argv[optind], "rt")) == NULL) {
            perror(argv[optind]);
            success=1;
            continue;
        }

        // This integer, firstrun, is set to 0 so that the loop will not print
        // anything before line2 is set to anything.
        int firstrun=0;

        // This while loop functions as long as there are lines in the file. For
        // each line, it is compared to the previous line (set to line2) and is 
        // only printed if they differ. In the event that they do not differ, 
        // the reps variable adds one to the count for that line. After every 
        // distinct line is printed, reps is set to a default value of 1.
        while(fgets(line1, 500, fr) != NULL) {
        	if (!(firstrun == 0) && (strcmp(line1, line2) != 0)) {
                if (c  == 1)
                    printf("%4d ", reps);
                // Removes the newline just in case a line is longer than 500 
                // bytes, in which case only one of the two lines will have a 
                // newline in it.
                noline(line2);
        		printf("%s\n", line2);
                reps=1;
            } else
                reps++;
            strcpy(line2, line1);
            firstrun=1;
        }

        // Due to the fact that the while loop checks for the file being null, 
        // the last line stored in line2 has yet to be printed and is printed 
        // out after the while loop has exited.
        if (c  == 1)
            printf("%4d ", reps);
        printf("%s", line2);

    }
    // The only way for the success value here to be 1 is for there to have been
    // an error reading a file, otherwise the program would return with a 
    // success value of 0 to indicate a successful job.
    return(success);
}

// No line will remove the newline from a string by setting the pointer
// currently set to a newline, to a termination, if there is one.
void noline (char *s) {
    while(*s && *s != '\n' && *s != '\r')
        s++;
    *s = 0;
}