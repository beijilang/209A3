#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

/* A program to model calling run_worker and to test it. Notice that run_worker
 * produces binary output, so the output from this program to STDOUT will
 * not be human readable.  You will need to work out how to save it and view
 * it (or process it) so that you can confirm that your run_worker
 * is working properly.
 */
int main(int argc, char **argv) {
    char ch;
    char path[PATHLENGTH];
    char *startdir = ".";
    /* this models using getopt to process command-line flags and arguments */
    while ((ch = getopt(argc, argv, "d:")) != -1) {
        switch (ch) {
        case 'd':
            startdir = optarg;
            break;
        default:
            fprintf(stderr, "Usage: queryone [-d DIRECTORY_NAME]\n");
            exit(1);
        }
    }

    // Open the directory provided by the user (or current working directory)
    DIR *dirp;
    if ((dirp = opendir(startdir)) == NULL) {
        perror("opendir");
        exit(1);
    }

    /* For each entry in the directory, eliminate . and .., and check
     * to make sure that the entry is a directory, then call run_worker
     * to process the index file contained in the directory.
     * Note that this implementation of the query engine iterates
     * sequentially through the directories, and will expect to read
     * a word from standard input for each index it checks.
     */
    struct dirent *dp;

    //create the sort records of size MAXRECORDS

    int fdword[MAXWORKERS][2], fdfreq[MAXWORKERS][2];
	int pipenum;
	int r = 0;
	int i = 0;
    while ((dp = readdir(dirp)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 ||
            strcmp(dp->d_name, "..") == 0 ||
            strcmp(dp->d_name, ".svn") == 0 ||
            strcmp(dp->d_name, ".git") == 0) {
                continue;
        }

        strncpy(path, startdir, PATHLENGTH);
        strncat(path, "/", PATHLENGTH - strlen(path));
        strncat(path, dp->d_name, PATHLENGTH - strlen(path));
        path[PATHLENGTH - 1] = '\0';

        struct stat sbuf;
        if (stat(path, &sbuf) == -1) {
            // This should only fail if we got the path wrong
            // or we don't have permissions on this entry.
            perror("stat");
            exit(1);
        }

        // Only call run_worker if it is a directory
        // Otherwise ignore it.


        if (S_ISDIR(sbuf.st_mode)) {
            pipe(fdword[i]);
            pipe(fdfreq[i]);
			pipenum = i;
            //parent
            if((r = fork())>0){

                //close the reading end of word
                if(close(fdword[i][0])== -1){
                    perror("close");
                    exit(1);
                }
                //close the writing end of freq
                if(close(fdfreq[i][1])== -1){
                    perror("close");
                    exit(1);
                }
                // close all the reading end of previous pipe
				/*fprintf(stderr,"%d",i);
                for(int j = 0; j < i; j++){
                    if(close(fdword[j][0])== -1){
		                perror("close reading end of previous child");
		                exit(1);
                    }
                }*/

            }else if(r == 0){
                //child
                //close the writing end of word
                if(close(fdword[i][1])== -1){
                    perror("close");
                    exit(1);
                }
                //close the reading end of freq
                if(close(fdfreq[i][0])== -1){
                    perror("close");
                    exit(1);
                }
                //close all the previous child reading end for freq
                for(int j = 0; j < i; j++){
                    if(close(fdfreq[j][0])== -1){
                    perror("close reading end of previous child");
                    exit(1);
                    }
                }
				break;
            }
            else{
                perror("fork");
                exit(1);
            }
            i++;
        }
    }

    // do all the work
    while(1){
        if(r > 0){
            FreqRecord records[MAXRECORDS];
            for(int i = 0; i < MAXRECORDS; i++){
                FreqRecord temp;
                temp.freq = 0;
                records[i]= temp;
            }
            FreqRecord record;
            char message[MAXWORD];

            fgets(message, MAXWORD, stdin);

			//write the word i times, so all children can read
			for(int j = 0; j < i; j++){
		        if(write(fdword[j][1],message,sizeof(char)*MAXWORD)==-1){
		            perror("write to pipe");
		        }
			}
			for(int j = 0; j < i; j++){
				fprintf(stderr,"%d\n", j);
		        while (read(fdfreq[j][0],record,sizeof(FreqRecord)) > 0) {
		            fprintf(stderr,"read something\n");
		            sort_freq_records(records, record);
            	}
			}
			print_freq_records(records);
        }
        else{
            run_worker(path,fdword[pipenum][0],fdfreq[pipenum][1]);
        }
    }

    if (closedir(dirp) < 0)
        perror("closedir");

    return 0;
}



