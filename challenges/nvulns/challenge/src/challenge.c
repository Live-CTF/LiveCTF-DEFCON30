#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <string.h>
#include <sys/sendfile.h>


typedef struct {
    char message_of_the_day[0x100];
} the_struct;

the_struct *uaf_struct;
size_t injection_size = 7;

long pretty_large_array[1];



uint64_t read_int()
{
    char input[0x100];
    fgets(input, 0x100, stdin);
    input[strcspn(input, "\n")] = 0;
    uint64_t result;
    sscanf(input, "%" SCNd64, &result);
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

void menu() {
    printf("What do you want to do?\n");
    printf("1. Stack bof\n");
    printf("2. Format string\n");
    printf("3. Command injection\n");

    printf("4. malloc\n");
    printf("5. free\n");
    printf("6. use\n");
    
    printf("7. SQL injection\n");
    printf("8. XSS\n");
    printf("9. Python deserialize\n");
    printf("10. Missing X-Frame-Options header\n");
    printf("11. Missing DMARC records\n");
    
    printf("12. Local File Inclusion\n");
    printf("13. Array out-of-bounds read\n");
    printf("14. Array out-of-bounds write\n");

    printf("> ");
}

int print_message(char *input) {
    printf("Your input: %s\n", input);
    exit(1);
    return 0;
}

int vuln_stack_bof() {
    char input[0x100];
    //gets(input); In memorium
    fgets(input, 0x4000, stdin);
    return print_message(input);
}

int vuln_format_string() {
    char input[0x400];
    fgets(input, sizeof(input), stdin);
    printf(input);
    return 0;
}

int vuln_command_injection() {
    char *input = malloc(injection_size);
    fgets(input, injection_size, stdin);
    char *command = malloc(injection_size + 16);
    sprintf(command, "unset PATH; echo \"%s\"", input);
    system(command);
    return 0;
}

int vuln_malloc() {
    uaf_struct = malloc(sizeof(uaf_struct));
    strcpy(uaf_struct->message_of_the_day, "This is a message!");
    return 0;
}

int vuln_free() {
    free(uaf_struct);
    return 0;
}

int vuln_use() {
    printf("MOTD: %s\n", uaf_struct->message_of_the_day);
    return 0;
}

int vuln_sql_injection() {
    printf("Nah, we called mysql_actually_real_for_sure_escape_string\n");
    return 0;
}

int vuln_xss() {
    printf("Imagine not using CSP in 2022. LMAO\n");
    return 0;
}

int vuln_python() {
    printf("Guido van Rossum told us we are not allowed to do this.\n");
    return 0;
}

int vuln_array_read() {
    size_t index = read_int();
    printf("Output: %lx\n", pretty_large_array[index]);
    return 0;
}

int vuln_array_write() {
    size_t index = read_int();
    long value = read_int();
    pretty_large_array[index] = value;
    return 0;
}

int vuln_local_file_inclusion() {
    char input[0x400];
    fgets(input, sizeof(input), stdin);
    int fd = open(input, O_RDONLY);
    sendfile(STDOUT_FILENO, fd, 0, 0x1000);
    close(fd);
    return 0;
}

int vuln_missing_xframe() {
    printf("Bounty plz?!\n");
    return 0;
}

int vuln_missing_dmarc() {
    printf("My proof is in the attached video: https://www.youtube.com/watch?v=dQw4w9WgXcQ\n");
    return 0;
}

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main(int argc, char** argv, char** envp)
{
    init();

    while(1) {
        menu();
        int choice = get_choice(14);
        switch(choice) {
            case 1:
            vuln_stack_bof();
            break;
            case 2:
            vuln_format_string();
            break;
            case 3:
            vuln_command_injection();
            break;
            case 4:
            vuln_malloc();
            break;
            case 5:
            vuln_free();
            break;
            case 6:
            vuln_use();
            break;
            case 7:
            vuln_sql_injection();
            break;
            case 8:
            vuln_xss();
            break;
            case 9:
            vuln_python();
            break;
            case 10:
            vuln_missing_xframe();
            break;
            case 11:
            vuln_missing_dmarc();
            break;
            case 12:
            vuln_local_file_inclusion();
            break;
            case 13:
            vuln_array_read();
            break;
            case 14:
            vuln_array_write();
            break;
        }
    }

    return 0;
}
