#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

uint64_t read_int(const char* message)
{
    char input[0x100];
    printf("%s\n", message);
    fgets(input, 0x100, stdin);
    input[strcspn(input, "\n")] = 0;
    return atoll(input);
}

int main(int argc, char** argv, char** envp)
{
    init();

    while (1) {
        uint64_t rdi = read_int("What do you want for rdi?");
        uint64_t rsi = read_int("What do you want for rsi?");
        uint64_t rdx = read_int("What do you want for rdx?");
        uint64_t r10 = read_int("What do you want for r10?");
        uint64_t r8 = read_int("What do you want for r8?");
        uint64_t r9 = read_int("What do you want for r9?");
        uint64_t syscall_num = read_int("What syscall number to call?");

        printf("syscall(%"PRIx64", %"PRIx64", %"PRIx64", %"PRIx64", %"PRIx64", %"PRIx64", %"PRIx64");\n", syscall_num, rdi, rsi, rdx, r10, r8, r9);

        uint64_t result = syscall(syscall_num, rdi, rsi, rdx, r10, r8, r9);

        printf("Syscall returned 0x%" PRIx64 "\n", result);
        if (result == -1) {
            perror("Syscall error");
        }

        if (read_int("Do another (0/1)?") == 0)
            break;
    }

    return 0;
}
