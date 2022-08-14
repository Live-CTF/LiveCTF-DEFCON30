#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void win(void)
{
    system("/bin/sh");
}

void init(void)
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

#define BF_PROGRAM_SIZE 9
#define BF_DATA_SIZE 128
#define BF_STACK_SIZE 16

typedef struct
{
    size_t ip;
    size_t dp;
    size_t depth;
    uint8_t stack[BF_STACK_SIZE];
    char program[BF_PROGRAM_SIZE];
    uint8_t data[BF_DATA_SIZE];
} bf_vm;

void bf_init(bf_vm *vm)
{
    vm->ip = 0;
    vm->dp = 0;
    vm->depth = 0;
    memset(vm->stack, 0, BF_STACK_SIZE);
    memset(vm->program, 0, BF_PROGRAM_SIZE);
    memset(vm->program, 0, BF_DATA_SIZE);
}

int main(int argc, char **argv, char **envp)
{
    init();

    bf_vm vm;
    bf_init(&vm);

    printf("Please input your brainf**k program: ");
    fgets(vm.program, BF_PROGRAM_SIZE, stdin);
    // int c;
    // while('\n' != (c = getchar()) && c != EOF);
    size_t program_len = strlen(vm.program);
    if (vm.program[program_len - 1] == '\n')
    {
        vm.program[program_len - 1] = 0;
    }
    printf("Program: \"%s\"\n", vm.program);

    while (vm.ip != BF_PROGRAM_SIZE)
    {
        //fprintf(stderr, "IP: %ld, INS: %c DP: %ld, [DP]: %hhd\n", vm.ip, vm.program[vm.ip], vm.dp, vm.data[vm.dp]);
        switch (vm.program[vm.ip])
        {
        case '>':
            vm.dp++;
            break;
        case '<':
            vm.dp--;
            break;
        case '+':
            vm.data[vm.dp]++;
            break;
        case '-':
            vm.data[vm.dp]--;
            break;
        case '.':
            putchar(vm.data[vm.dp]);
            break;
        case ',':
            int c = getchar();
            //fprintf(stderr, "In: \"%c\" -> [%ld]\n", c, vm.dp);
            if (c != EOF)
            {
                vm.data[vm.dp] = c;
            }
            break;
        case '[':
            vm.depth++;
            if (vm.data[vm.dp] == 0)
            {
                int left = 0;
                int right = 0;
                for (size_t new_ip = vm.ip;; new_ip++)
                {
                    if (vm.program[new_ip] == '[')
                        left++;
                    if (vm.program[new_ip] == ']')
                        right++;

                    //fprintf(stderr, "L: %d, R: %d\n", left, right);

                    if (left == right)
                    {
                        vm.ip = new_ip;
                        break;
                    }
                }
            }
            else
            {
                vm.stack[vm.depth++] = vm.ip;
            }
            break;
        case ']':
            if (vm.data[vm.dp] == 0)
            {
                vm.depth--;
            }
            else
            {
                vm.ip = vm.stack[vm.depth - 1];
            }
            break;

        case '^':
            return 0;
            break;
        default:
            break;
        }
        vm.ip++;
    }
    fflush(stdout);

    return 0;
}
