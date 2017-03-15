//
// Created by sirius on 15.03.17.
//

#include "../include/liboperations.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void lib_error_dump(){
    fprintf(stderr, "An error has occured during library file function %s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

void lib_generate(char *fname, uint32_t record_count, uint32_t record_size) {
    FILE *frandom, *ftarget;
    if (!(frandom = fopen("/dev/urandom", "r")))
        lib_error_dump();

    if (!(ftarget = fopen(fname, "w")))
        lib_error_dump();

    unsigned char* buffer = malloc(record_size * sizeof(unsigned char));

    for (int i = 0; i < record_count; i++){
        if (fread(buffer, sizeof(unsigned char), record_size, frandom) <= 0)
            lib_error_dump();
        if (fwrite(buffer, sizeof(unsigned char), record_size, ftarget) <= 0)
            lib_error_dump();
    }

    free(buffer);
    if (fclose(ftarget) == EOF || fclose(frandom) == EOF)
        lib_error_dump();

}

void lib_sort(char *fname, uint32_t record_count, uint32_t record_size) {
    FILE *file;
    if (!(file = fopen(fname, "r+")))
        lib_error_dump();

    unsigned char *first, *second;
    first = malloc(record_size * sizeof(unsigned char));
    second = malloc(record_size * sizeof(unsigned char));

    uint32_t fsize = record_count * record_size;

    for (uint32_t i = 0; i < fsize; i += record_size){
        for (uint32_t j = 0; j < fsize - record_size; j += record_size){
            if (fseek(file, j, SEEK_SET) == -1
                || fread(first, sizeof(unsigned char), record_size, file) <= 0)
                lib_error_dump();
            if (fseek(file, j+record_size, SEEK_SET) == -1
                || fread(second, sizeof(unsigned char), record_size, file) <= 0)
                lib_error_dump();

            if (*first > *second){
                //swap records
                if (fseek(file, j+record_size, SEEK_SET) == -1
                    || fwrite(first, sizeof(unsigned char), record_size, file) <= 0)
                    lib_error_dump();
                if (fseek(file, j, SEEK_SET) == -1
                    || fwrite(second, sizeof(unsigned char), record_size, file) <= 0)
                    lib_error_dump();
            }


        }
    }

    free(first);
    free(second);
    if (fclose(file) == EOF) {
        lib_error_dump();
    }
}

void lib_shuffle(char *fname, uint32_t record_count, uint32_t record_size) {
    FILE *file;
    if (!(file = fopen(fname, "r+")))
        lib_error_dump();

    int j;
    unsigned char *first, *second;
    first = malloc(record_size*sizeof(unsigned char));
    second = malloc(record_size*sizeof(unsigned char));
    for (int i = record_count-1; i > 0; i--){
        j = rand() % (i+1);

        if (fseek(file, i*record_size, SEEK_SET) == -1
            || fread(first, sizeof(unsigned char),  record_size, file) <= 0)
            lib_error_dump();
        if (fseek(file, j*record_size, SEEK_SET) == -1
            || fread(second, sizeof(unsigned char),  record_size, file) <= 0)
            lib_error_dump();

        //exchange
        if (fseek(file, j*record_size, SEEK_SET) == -1
            || fwrite(first, sizeof(unsigned char), record_size, file) <= 0)
            lib_error_dump();
        if (fseek(file, i*record_size, SEEK_SET) == -1
            || fwrite(second, sizeof(unsigned char), record_size, file) <= 0)
            lib_error_dump();
    }

    free(first);
    free(second);
    if (fclose(file) == EOF){
        lib_error_dump();
    }
}
