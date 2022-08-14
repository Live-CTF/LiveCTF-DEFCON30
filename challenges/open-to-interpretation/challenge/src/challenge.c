#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define PROGRAM_MAX_SIZE UINT16_MAX

#define STORAGE_SIZE 128

/*
Custom interpreter for simple language
    - brainfuck-esque
*/
// send a program of > INT16_MAX len
// Bug: you can get position to a negative index bc
struct PROGRAM {
    char* contents;
    uint8_t storage[128];
    int8_t position;
};


struct PROGRAM program;


void fake_system(void)
{
    system("echo not that easy!");
}


void init(void)
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}


void read_program()
{
    fgets(program.contents, PROGRAM_MAX_SIZE, stdin);
    program.contents[strcspn(program.contents, "\n")] = 0;
}


void clear_program()
{
    if (program.contents != NULL)
    {
        free(program.contents);
    }
    memset(&program, 0, sizeof(program));
    program.contents = calloc(PROGRAM_MAX_SIZE, 1);
}


__attribute__((optnone)) void execute_program() // hehehe
{
    int program_len = strlen(program.contents);
    for (int i = 0; i < program_len; i++)
    {
        char c = program.contents[i];
        switch (c)
        {
            case 'w':
            //inc
                program.storage[program.position] += 1;
                break;
            case 's':
            //dec
                program.storage[program.position] -= 1;
                break;
            case 'a':
            //left
                program.position -= 1;
                if (program.position < 0)
                {
                    puts("Out of bounds");
                    return;
                }
                break;
            case 'd':
            //right
                program.position += 1;
                if (program.position == STORAGE_SIZE)
                {
                    puts("Out of bounds");
                    return;
                }
                break;
            case '?':
            //print
                putchar(program.storage[program.position]);
                break;
            case 'x':
            //print
                fake_system();
                return;
            default:
                puts("Bad instruction");
                return;
        }
    }
}


int main(int argc, char** argv, char** envp)
{
    init();

    while (1)
    {
        clear_program();
        puts("Give me a program: ");
        read_program();
        execute_program();
        puts("Run another? (y/n)");
        if (fgetc(stdin) != 'y')
            break;
    }
    return 0;
}
