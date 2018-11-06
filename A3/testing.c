#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>

#include "freq_list.h"
#include "worker.h"

int main(int argc, char **argv) {
    Node *head = NULL;
    char **filenames = init_filenames();
    char arg;
    char *listfile = "index";
    char *namefile = "filenames";

    /* an example of using getop to process command-line flags and arguments */
    while ((arg = getopt(argc,argv,"i:n:")) > 0) {
        switch(arg) {
        case 'i':
            listfile = optarg;
            break;
        case 'n':
            namefile = optarg;
            break;
        default:
            fprintf(stderr, "Usage: printindex [-i FILE] [-n FILE]\n");
            exit(1);
        }
    }

    read_list(listfile, namefile, &head, filenames);
    display_list(head, filenames);
    FreqRecord* record = get_word("Banada",head,filenames);
    print_freq_records(record);

    return 0;
}
