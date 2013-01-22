#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

DIR *dp;
// Flags and parameters are initialized for the possible arguments.
// - The fflag is either on or off so that's all that's neccessary.
// - The m integer stores the maximum number of days passed since modification.
// - The d integer tracks the current depth of the search, set to -2 by default.
// - The s integer tracks the minimum file size, with a dault of 0 bytes.
// - The fail integer tracks whether the program has run into an error or not.
static int fflag = 1, d = 32767, m = 0, s = 0, fail = 0;

// The fullpath character array is initiallized to store the path and the 
// pathsize is initialized to store the length the fullpath array.
char *fullpath = NULL;
int pathsize = 0;

extern void myrealloc(int size);
extern void printdircontents();

int main(int argc, char **argv)
{
    // An integer is initialized to store the specified arguments passed from
    // getopt.
    int c;

    // getopt parses the arguments as discussed at the beggining of the file.
    while ((c = getopt(argc, argv, "s:d:m:f")) != EOF) {
        switch (c) {
        case 'f':
            fflag = 0;
            break;
        case 'd':
            d = atoi(optarg);
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'm':
            m = time(0) - (60*60*24)*atoi(optarg);
            break;
        default:
            fail=1;
            break;
        }
    }

    // if not arguments have been passed to myfind, then the fail variable is 
    // set to 1.
    if (optind >= argc) {
        fail=1;
    }

    // If the fail variable is set to 1, due to no arguments or irregular 
    // options, the the program exits with a 1 and sends a proper usage message
    // to the standard error.
    if (fail) {
        fprintf(stderr,"usage: %s [-f] [-s bytesize] [-d maxdepth] [-m mtimedays] dir ...\n", argv[0]);
        return(fail);
    }

    // The size of the fullpath variable is set to 256.
    myrealloc(256);

    // Each of the arguments are iterated over by myfind.
    for(; optind < argc; optind++) {
        strcpy(fullpath,argv[optind]);
        printdircontents();
    }
    // A value of 0 is returned to indicate a successful run of the program.
    return(fail);
}

// The reallocation program is taken from the CSC09 website, written by 
// professor Alan J Rosenthal which takes care of the resizing of the character 
// array which stores the full path to the file.
void myrealloc(int size)
{
    if (size <= pathsize)
        return;
    pathsize = size;
    if ((fullpath = realloc(fullpath, pathsize)) == NULL) {
        fprintf(stderr, "out of memory!\n");
        exit(1);
    }
}

// Prints the contents of a directory and its subdirectories.
void printdircontents()
{   
    DIR * dp;

    // Failing to open the directory does not end the program and it continues
    // to look in previous directories.
    if ((dp = opendir(fullpath)) == NULL) {
        perror(fullpath);
        fail=1;
        return;
    }
    
    // An int to store wether or not an item is a directory and if it should be
    // printed or not.
    // The fullpath is reallocated 255 bytes more than its current length.
    struct dirent * currentdir;
    int adir=0;
    int shouldprint;
    myrealloc(strlen(fullpath)+255);

    // This loop occurs for every object in the folder.
    while ((currentdir = readdir(dp)) && currentdir) {
        // This if statement stops the program from recursing on the current 
        // directory or the parent directory by skipping the iterations when it
        // reads '.' and '..'.
        if ( (strcmp(currentdir->d_name, ".") == 0) || 
            (strcmp(currentdir->d_name, "..")) == 0 )
            continue;

        // Each item is given a fault value indicating it should be printed to
        // the standard output and each item is given a value, adir, storing
        // whether it is a directory or not.
        adir=currentdir->d_type & DT_DIR;
        shouldprint=0;

        // For each file, the fullpath is updated to reflect the path to, and 
        // including, the file.
        strcat(fullpath, "/");
        strcat(fullpath, currentdir->d_name);

        // If the item is a directory and the fflag indicated not to print 
        // directories, it is marked to not be printed.
        if (adir && !(fflag))
            shouldprint=1;

        // This checks if the files meet the filesize and timelimit parameters.
        if (shouldprint == 0) {
            struct stat sb;
            if (lstat(fullpath, &sb) >= 0) {
                if ((s != 0) && (sb.st_size < s))
                    shouldprint=1;
                if (m > sb.st_mtime)
                    shouldprint=1;
            }
        }

        // The item is printed unless it did not pass one of the above tests.
        if (shouldprint == 0)
            printf("%s\n", fullpath);

        // If the item is a direcory and is not passed the recursion depth, if 
        // specified by the user, then it is recursed upon and the depth count 
        // is increased by 1 before recursing into the directory.
        if ((adir) && (d > 0)) {
            d=d-1;
            printdircontents();
            d=d+1;
        }
        // Once an item has been iterated over, the fullpath variable is reset 
        // to the path without the file on the end.
        fullpath[strlen(fullpath)-strlen(currentdir->d_name) - 1] = '\0';
    }
    // The directory is closed after reading.
    closedir(dp);
}