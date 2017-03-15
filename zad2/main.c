#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>

void dump_errno(){
    if (errno != 0)
        fprintf(stderr, "An error has occured: %s\n", strerror(errno));
    else
        fprintf(stderr, "An unknown error has occured. Aborting.\n");
    exit(EXIT_FAILURE);
}

void print_file_permissions(mode_t mode){
    printf("%s%s%s%s%s%s%s%s%s",
           (mode & S_IRUSR) ? "r" : "-",
           (mode & S_IWUSR) ? "w" : "-",
           (mode & S_IXUSR) ? "x" : "-",
           (mode & S_IRGRP) ? "r" : "-",
           (mode & S_IWGRP) ? "w" : "-",
           (mode & S_IXGRP) ? "x" : "-",
           (mode & S_IROTH) ? "r" : "-",
           (mode & S_IWOTH) ? "w" : "-",
           (mode & S_IXOTH) ? "x" : "-"
    );
}

void dirent_descend(char* dirname, int max_fsize){
    printf("=> %s\n", dirname);
    DIR* dir = opendir(dirname);
    if (!dir) dump_errno();
    struct dirent* curr;
    struct stat* curr_file = malloc(sizeof(struct stat));
    char* date_buff = malloc(26*sizeof(char));
    while ((curr = readdir(dir))){
        if (strcmp(curr->d_name, ".") == 0
                || strcmp(curr->d_name, "..") == 0)
            continue;

        uint32_t pathsize = (uint32_t)strlen(curr->d_name) + (uint32_t)strlen(dirname) + 2;
        char* fpath = malloc(pathsize);
        sprintf(fpath, "%s/%s", dirname, curr->d_name);

        lstat(fpath, curr_file);

        if (S_ISREG(curr_file->st_mode)){
            uint32_t fsize = (uint32_t)curr_file->st_size;
            if (fsize > max_fsize)
                continue;
            ctime_r(&(curr_file->st_mtim.tv_sec), date_buff);
            printf("- %s %d bytes ", fpath, fsize);
            print_file_permissions(curr_file->st_mode);
            printf(" modified: %s", date_buff);
        }
        if (S_ISDIR(curr_file->st_mode) && !S_ISLNK(curr_file->st_mode))
            dirent_descend(fpath, max_fsize);
        free(fpath);
    }
    free(date_buff);
    free (curr_file);

    if (closedir(dir) == -1)
        dump_errno();
    printf("<=\n");
}

int main(int argc, char *argv[]){

    if (argc != 3){
        printf("Incorrect number of parameters\n");
        printf("Usage: <path> <size>\n");
        exit(EXIT_FAILURE);
    }

    uint32_t max_fsize = 0;
    char *endptr;
    max_fsize = (uint32_t)strtoul(argv[2], &endptr, 10);
    if (*endptr != '\0' || max_fsize == 0){
        printf("Incorrect max file size number: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char* rpath;
    if (!(rpath = realpath(argv[1], NULL)))
        dump_errno();

    dirent_descend(rpath, max_fsize);
    free(rpath);
    return 0;
}