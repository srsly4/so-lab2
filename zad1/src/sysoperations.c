//
// Created by sirius on 14.03.17.
//

#include "../include/sysoperations.h"
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void sys_error_dump(){
    fprintf(stderr, "Could have not complete the system file function: %s", strerror(errno));
    exit(EXIT_FAILURE);
}

void sys_generate(char *fname, uint32_t record_count, uint32_t record_size) {
    int frandom, ftarget;

    if ((frandom = open("/dev/urandom", O_RDONLY)) == -1)
        sys_error_dump();

    if ((ftarget = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0755)) == -1)
        sys_error_dump();

    unsigned char* buffer = malloc(record_size * sizeof(unsigned char));

    for (int i = 0; i < record_count; i++){
        if (read(frandom, buffer, record_size) == -1)
            sys_error_dump();
        if (write(ftarget, buffer, record_size) == -1)
            sys_error_dump();
    }

    free(buffer);
    if (close(ftarget) == -1 || close(frandom) == -1)
        sys_error_dump();
}

void sys_sort(char *fname, uint32_t record_count, uint32_t record_size) {
    int file;
    if ((file = open(fname, O_RDWR)) == -1)
        sys_error_dump();

    unsigned char *first, *second;
    first = malloc(record_size * sizeof(unsigned char));
    second = malloc(record_size * sizeof(unsigned char));

    uint32_t fsize = record_count * record_size;

    for (uint32_t i = 0; i < fsize; i += record_size){
        for (uint32_t j = 0; j < fsize - record_size; j += record_size){
            if (lseek(file, j, SEEK_SET) == -1
                || read(file, first, record_size) <= 0)
                sys_error_dump();
            if (lseek(file, j+record_size, SEEK_SET) == -1
                || read(file, second, record_size) <= 0)
                sys_error_dump();

            if (*first > *second){
                //swap records
                if (lseek(file, j+record_size, SEEK_SET) == -1
                    || write(file, first, record_size) <= 0)
                    sys_error_dump();
                if (lseek(file, j, SEEK_SET) == -1
                    || write(file, second, record_size) <= 0)
                    sys_error_dump();
            }


        }
    }

    free(first);
    free(second);
    if (close(file)) {
        sys_error_dump();
    }
}

void sys_shuffle(char *fname, uint32_t record_count, uint32_t record_size) {
    int file;
    if ((file = open(fname, O_RDWR)) == -1)
        sys_error_dump();

    int j;
    unsigned char *first, *second;
    first = malloc(record_size*sizeof(unsigned char));
    second = malloc(record_size*sizeof(unsigned char));
    for (int i = record_count-1; i > 0; i--){
        j = rand() % (i+1);

        if (lseek(file, i*record_size, SEEK_SET) == -1
            || read(file, first, record_size) <= 0)
            sys_error_dump();
        if (lseek(file, j*record_size, SEEK_SET) == -1
            || read(file, second, record_size) <= 0)
            sys_error_dump();

        //exchange
        if (lseek(file, j*record_size, SEEK_SET) == -1
            || write(file, first, record_size) <= 0)
            sys_error_dump();
        if (lseek(file, i*record_size, SEEK_SET) == -1
            || write(file, second, record_size) <= 0)
            sys_error_dump();
    }

    free(first);
    free(second);
    if (close(file)){
        sys_error_dump();
    }

}
