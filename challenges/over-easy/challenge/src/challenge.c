#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

static int eggs = 0;

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int num(void) {
    char line[256];
    if (!fgets(line, 256, stdin))
        exit(0);
    return atoi(line);
}

bool menu(void) {
    printf("You have counted %d egg(s).\n\n", eggs);

    puts("How many groups of eggs are you counting?");
    int count = num();
    if (count <= 0)
        return false;

    unsigned char groups[32];
    for (int i = 0; i < count; i++) {
        printf("Number of eggs in group %d: ", i + 1);
        fflush(stdout);
        groups[i] = (unsigned char)num();
    }

    puts("Tallying...");
    for (int i = 0; i < count; i++) {
        eggs += groups[i];
    }

    puts("Done\n");
    return true;
}

int main(int argc, char** argv, char** envp)
{
    init();

    while (menu()) {}
    return 0;
}

void win(void)
{
    puts("Egg shells incoming...");
    char* args[2] = {"/bin/sh", NULL};
    execve(args[0], args, NULL);
}
