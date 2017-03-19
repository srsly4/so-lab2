#ifdef USE_NFTW
#define _XOPEN_SOURCE 500
#define __USE_XOPEN_EXTENDED 1
#include <ftw.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <time.h>
#ifndef USE_NFTW
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif

char* date_buff;
uint32_t nftw_max_fsize;

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
#ifndef USE_NFTW
void dirent_descend(char* dirname, int max_fsize){
    printf("=> %s\n", dirname);
    DIR* dir = opendir(dirname);
    if (!dir){
        printf("=> Cannot descend into: %s - %s \n", dirname, strerror(errno));
        return;
    }
    struct dirent* curr;
    struct stat* curr_file = malloc(sizeof(struct stat));

    char* fpath = malloc(PATH_MAX);
    while ((curr = readdir(dir))){
        if (strcmp(curr->d_name, ".") == 0
                || strcmp(curr->d_name, "..") == 0) {
            continue;
        }

        sprintf(fpath, "%s/%s", dirname, curr->d_name);

        if (lstat(fpath, curr_file) == -1)
        {
            fprintf(stderr, "Could not lstat file %s\n", fpath);
            break;
        }

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
    }
    free(fpath);
    free (curr_file);

    if (closedir(dir) == -1)
        dump_errno();
    printf("<=\n");
}

#endif

#ifdef USE_NFTW
int nftwfunc(const char *filename, const struct stat *statptr,
             int fileflags, struct FTW *pfwt){

    if ((fileflags & FTW_F) || S_ISREG(statptr->st_mode))
    {
        ctime_r(&(statptr->st_mtime), date_buff);
        uint32_t size = (uint32_t)(statptr->st_size);
        if (size > nftw_max_fsize)
            return 0;
        printf("%s %d bytes ", filename, size);
        print_file_permissions(statptr->st_mode);
        printf(" modified: %s", date_buff);
    }
    else if ((fileflags & FTW_NS) && S_ISREG(statptr->st_mode)){
        printf("%s [ACCESS DENIED]\n", filename);
    }
    return 0;
}
#endif

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

    date_buff = malloc(26*sizeof(char));
#ifndef USE_NFTW
    dirent_descend(rpath, max_fsize);
#else
    nftw_max_fsize = max_fsize;
    nftw(rpath, nftwfunc, 1024, FTW_PHYS);
#endif
    free(date_buff);
    free(rpath);
    return 0;
}