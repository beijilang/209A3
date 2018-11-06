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


    read_list(listfile, namefile, &head, filenames);
    display_list(head, filenames);
    FreqRecord* record = get_word(argv[1],head,filenames);
    print_freq_records(record);

    return 0;
}
