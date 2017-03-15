#include <stdio.h>
#include "../include/sysoperations.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

void (*tpl_generate)(char *fname, uint32_t record_count, uint32_t record_size);
void (*tpl_sort)(char *fname, uint32_t record_count, uint32_t record_size);
void (*tpl_shuffle)(char *fname, uint32_t record_count, uint32_t record_size);

void select_mode_sys(){
    tpl_generate = &sys_generate;
    tpl_sort = &sys_sort;
    tpl_shuffle = &sys_shuffle;
}

void select_mode_lib(){

}

int main(int argc, char *argv[]) {

    if (argc != 6)
    {
        printf("Incorrect number of arguments.\n");
        printf("Usage: <sys|lib> <generate|shuffle|sort> <filename> <count> <size>\n");
        exit(0);
    }

    //parse library mode
    if (strcmp(argv[1], "sys") == 0)
    {
        select_mode_sys();
    }
    else if (strcmp(argv[1], "lib") == 0){
        select_mode_lib();
    }
    else {
        printf("Incorrect library mode used.\n");
        exit(0);
    }

    //parse operation mode
    uint32_t rcount = 0, rsize = 0;
    char *endptr;
    rcount = (uint32_t)strtoul(argv[4], &endptr, 10);
    if (*endptr != '\0' || rcount == 0){
        printf("Incorrect value of record count number: %s", strerror(errno));
        exit(0);
    }

    rsize = (uint32_t)strtoul(argv[5], &endptr, 10);
    if (*endptr != '\0' || rsize == 0){
        printf("Incorrect value record size number: %s", strerror(errno));
        exit(0);
    }

    char* operation = argv[2];
    if (strcmp(operation, "generate") == 0){
        tpl_generate(argv[3], rcount, rsize);
    }
    else if (strcmp(operation, "sort") == 0){
        tpl_sort(argv[3], rcount, rsize);
    }
    else if (strcmp(operation, "shuffle") == 0){
        tpl_shuffle(argv[3], rcount, rsize);
    }

    return 0;
}