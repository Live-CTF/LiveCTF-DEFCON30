#include <stdio.h>
#include <stdlib.h>

void win(void)
{
    system("/bin/sh");
}

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main(int argc, char** argv, char** envp)
{
    init();

    char buf[32] = { 0 };
    puts("Give me input: ");
    gets(buf);
    printf("You sent: %s\n", buf);
    return 0;
}
