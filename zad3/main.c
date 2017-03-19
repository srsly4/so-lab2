#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>

void print_main_options(void){
    printf("1. Ustaw rygiel do odczytu na wybrany znak\n");
    printf("2. Ustaw rygiel do zapisu na wybrany znak\n");
    printf("3. Wyświetl listę zaryglowanych znaków w pliku\n");
    printf("4. Zwolnij wybrany rygiel\n");
    printf("5. Odczytaj wybrany znak pliku\n");
    printf("6. Zapisz wybrany znak pliku\n");
    printf("0. Wyjdź\n");
    printf("=> ");
}

bool want_waiting(){
    printf("Wait to get access (y/n)? ");
    char d = '0';
    while (d != 'y' && d != 'n')
    {
        scanf(" %c", &d);
    }
    return d == 'y';
}

void dump_err(void){
    fprintf(stderr, "An error has occured: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

void set_read_lock(int fhndl, uint32_t pos, bool wait){
    struct flock lck;
    lck.l_type = F_RDLCK;
    lck.l_len = 1;
    lck.l_start = pos;
    lck.l_whence = SEEK_SET;
    if (fcntl(fhndl, wait ? F_SETLKW : F_SETLK, &lck) == -1)
        printf("Error: %s\n", strerror(errno));
    else
        printf("Lock set succesfully\n");
}

void set_write_lock(int fhndl, uint32_t pos, bool wait){
    struct flock lck;
    lck.l_type = F_WRLCK;
    lck.l_len = 1;
    lck.l_start = pos;
    lck.l_whence = SEEK_SET;
    if (fcntl(fhndl, wait ? F_SETLKW : F_SETLK, &lck) == -1)
        printf("Error: %s\n", strerror(errno));
    else
        printf("Lock set succesfully\n");
}

void unlock_byte(int fhndl, uint32_t pos){
    struct flock lck;
    lck.l_type = F_UNLCK;
    lck.l_len = 1;
    lck.l_start = pos;
    lck.l_whence = SEEK_SET;
    if (fcntl(fhndl, F_SETLK, &lck) == -1)
        printf("Error: %s\n", strerror(errno));
    else
        printf("Lock unset succesfully\n");
}

void set_byte(int fhndl, uint32_t pos, char chr){
    lseek(fhndl, pos, SEEK_SET);
    if (write(fhndl, &chr, sizeof(char)) != -1)
        printf("Byte was succesfully written!\n");
    else{
        printf("Error during writing: %s\n", strerror(errno));
    }
}

char read_byte(int fhndl, uint32_t pos){
    lseek(fhndl, pos, SEEK_SET);
    char chr;
    if (read(fhndl, &chr, sizeof(char)) != -1){
        printf("Byte was succesfully read!\n");
    return chr;
    }
    else{
        printf("Error during reading: %s\n", strerror(errno));
        return '\0';
    }
}

void list_locks(int fhndl){
    uint32_t fsize = (uint32_t)lseek(fhndl, 0, SEEK_END) + 1;
    struct flock *lck = malloc(sizeof(lck));
    for (uint32_t pos = 0; pos < fsize; pos++){
        lseek(fhndl, pos, SEEK_SET);
        lck->l_whence = SEEK_SET;
        lck->l_start = pos;
        lck->l_len = 1;
        lck->l_type = F_RDLCK;
        fcntl(fhndl, F_GETLK, lck);

        if (lck->l_type != F_UNLCK)
            printf("Read lock on offset %d by PID %d\n", pos, lck->l_pid);

        lck->l_whence = SEEK_SET;
        lck->l_start = pos;
        lck->l_len = 1;
        lck->l_type = F_WRLCK;
        fcntl(fhndl, F_GETLK, lck);

        if (lck->l_type != F_UNLCK)
            printf("Write lock on offset %d by PID %d\n", pos, lck->l_pid);
    }

    free(lck);
}

int main(int argc, char *argv[]) {

    if (argc != 2)
    {
        printf("Incorrect number of arguments.\nUsage: <filename>\n");
        exit(EXIT_FAILURE);
    }

    int fhndl = open(argv[1], O_RDWR | O_EXCL);
    if (fhndl == -1) dump_err();

    int option;
    print_main_options();
    while ((option = getchar()) != '0'){
        if (option == '\n' || option == EOF) continue;
        bool waiting = false;
        uint32_t offset = 0;
        switch(option){
            case '1':
                printf("\nEnter the offset: ");
                offset = 0;
                scanf("%d", &offset);
                printf("\n");
                waiting = want_waiting();
                set_read_lock(fhndl, offset, waiting);
                break;
            case '2':
                printf("\nEnter the offset: ");
                offset = 0;
                scanf("%d", &offset);
                printf("\n");
                waiting = want_waiting();
                set_write_lock(fhndl, offset, waiting);
                break;
            case '3':
                printf("\nFound below-listed locks: \n");
                list_locks(fhndl);
                printf("\n");
                break;
            case '4':
                printf("\nEnter the offset: ");
                offset = 0;
                scanf("%d", &offset);
                printf("\n");
                unlock_byte(fhndl, offset);
                break;
            case '5':
                printf("\nEnter the offset: ");
                offset = 0;
                scanf("%d", &offset);
                char read = read_byte(fhndl, offset);
                if (read != '\0')
                    printf("Read char: %c\n", read);
                option = 5;
                break;
            case '6':
                printf("\nEnter the offset: ");
                offset = 0;
                scanf("%d", &offset);
                printf("\nEnter the character to write: ");
                char written = '\0';
                scanf(" %c", &written);
                set_byte(fhndl, offset, written);
                printf("\n");
                break;
            default:
                break;
        }
        printf("\n");
        print_main_options();
    }

    close(fhndl);
    return 0;
}