#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

static const char* last_file = NULL;

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void menu() {
    printf("What do you want to do?\n");
    printf("1. Open file\n");
    printf("2. Close file\n");
    printf("3. Write to file\n");
    printf("4. Read from file\n");
    printf("> ");
}

uint64_t read_int()
{
    char input[0x100];
    fgets(input, 0x100, stdin);
    input[strcspn(input, "\n")] = 0;
    uint64_t result;
    sscanf(input, "%" SCNx64, &result);
    return result;
}

int get_choice(int max) {
    while(1) {
        int choice;

        choice = read_int();
        if(choice > 0 && choice <= max) {
            return choice;
        }

        printf("Invalid choice\n");
    }
}

void open_file(FILE **cur_file) {
    char filename[0x100];
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;

    char mode[16];
    fgets(mode, sizeof(mode), stdin);

    *cur_file = fopen(filename, mode);
    if(*cur_file == NULL) {
        printf("Failed to open file.\n");
        return;
    }
    last_file = strdup(filename);
}

void close_file(FILE **cur_file) {
    if(*cur_file == NULL) {
        printf("You do not have a file open. Open one first.\n");
        return;
    }
    if(fclose(*cur_file)) {
        printf("Failed to close file.\n");
    }
}

void write_file(FILE **cur_file) {
    if(*cur_file == NULL) {
        printf("You do not have a file open. Open one first.\n");
        return;
    }

    if (freopen(last_file, "wb", *cur_file) == NULL) {
        printf("Could not reopen file for writing.\n");
        return;
    }

    printf("Where in the file do you want to write? ");
    size_t offset = read_int();
    printf("How much do you want to write? ");
    size_t data_len = read_int();
    if(fseek(*cur_file, offset, SEEK_SET)) {
        printf("Failed to seek in file.\n");
        return;
    }
    unsigned char *buf = malloc(data_len);
    if(buf == NULL) {
        printf("Failed to allocate buffer.\n");
        return;
    }
    if(data_len != fread(buf, sizeof(unsigned char), data_len, stdin)) {
        printf("Failed to read data.\n");
        free(buf);
        return;
    }
    if(data_len != fwrite(buf, sizeof(unsigned char), data_len, *cur_file)) {
        printf("Failed to write data to file.\n");
        free(buf);
        return;
    }
    fflush(*cur_file); // Easy mode for people who don't know about buffered I/O
    free(buf);
}

void read_file(FILE **cur_file) {
    if(*cur_file == NULL) {
        printf("You do not have a file open. Open one first.\n");
        return;
    }

    printf("Where in the file do you want to read? ");
    size_t offset = read_int();
    printf("How much do you want to read? ");
    size_t data_len = read_int();
    if(fseek(*cur_file, offset, SEEK_SET)) {
        printf("Failed to seek in file.\n");
        return;
    }
    unsigned char *buf = malloc(data_len);
    if(buf == NULL) {
        printf("Failed to allocate buffer.\n");
        return;
    }
    if(data_len != fread(buf, sizeof(unsigned char), data_len, *cur_file)) {
        printf("Failed to read data from file.\n");
        free(buf);
        return;
    }
    if(data_len != fwrite(buf, sizeof(unsigned char), data_len, stdout)) {
        printf("Failed to write data.\n");
        free(buf);
        return;
    }
    free(buf);
}

int main(int argc, char** argv, char** envp)
{
    init();
    FILE *cur_file = NULL;

    while(1) {
        menu();
        int choice = get_choice(4);
        switch(choice) {
            case 1:
            open_file(&cur_file);
            break;
            case 2:
            close_file(&cur_file);
            break;
            case 3:
            write_file(&cur_file);
            break;
            case 4:
            read_file(&cur_file);
            break;
        }
    }

    return 0;
}
