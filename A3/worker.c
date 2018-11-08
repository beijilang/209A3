#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

#define READSIZE 128
/* return an array of FreqRecord that contains the occurrence of word in each file
the last index of FreqRecord would have 0 as freq and '\0' as filename
*/
FreqRecord *get_word(char *word, Node *head, char **file_names) {
    Node* cur = head;
    int found = 0;
    while(cur != NULL){
        if(strcmp(cur->word, word) == 0){
            found = 1;
            break;
        }
        cur = cur->next;
    }
    int i = 0;
    while(file_names[i] != NULL){
        i++;
    }
    if(!found){
        i = 0;
    }

    //store the last one
    FreqRecord* record = malloc(sizeof(FreqRecord)*(i+1));
    FreqRecord tail;
    tail.freq = 0;
    tail.filename[0]='\0';
    record[i] = tail;
    int j;
    for(j = 0; j<i;j++){
        FreqRecord temp;
        temp.freq = cur->freq[j];
        strcpy(temp.filename, file_names[j]);
        record[j] = temp;
        printf("name:%s\n",file_names[j]);
    }
    return record;
}

/* Print to standard output the frequency records for a word.
* Use this for your own testing and also for query.c
*/
void print_freq_records(FreqRecord *frp) {
    int i = 0;
    while (frp != NULL && frp[i].freq != 0) {
        printf("%d    %s\n", frp[i].freq, frp[i].filename);
        i++;
    }
}

/* Complete this function for Task 2 including writing a better comment.
*/
void run_worker(char *dirname, int in, int out) {
    Node *head = NULL;
    char **filenames = init_filenames();
    char listfile[128];
    char namefile[READSIZE];
    strcpy(listfile,dirname);
    strcpy(namefile,dirname);
    strncat(listfile,"/index",READSIZE);
    strncat(namefile,"/filenames",READSIZE);
    read_list(listfile, namefile, &head, filenames);

    int i = 0;
    char received[READSIZE];
    display_list(head, filenames);
    while((read(in,received,READSIZE))>0){
        FreqRecord* record = get_word(received,head,filenames);
        print_freq_records(record);
        while (1){
            if(record[i].freq == 0 && strcmp(record[i].filename,"")==0){
                if(write(out,record[i].filename,sizeof(FreqRecord))==-1){
                    perror("write to pipe");
                };
                break;
            }
            if(write(out,record[i].filename,sizeof(FreqRecord))==-1){
                perror("write to pipe");
            };

        }
    }



    return;
}
