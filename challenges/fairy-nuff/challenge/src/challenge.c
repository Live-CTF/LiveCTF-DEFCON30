#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <string.h>

uint8_t brew[256] = {0};

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

    puts("I am Fairy Nuff, please help me brew my Magic Essence potion!\n");

    int spot = 0;
    while (true)
    {
        puts("What next?");
        puts("1) Stir to the left");
        puts("2) Stir to the right");
        puts("3) Add in a little Starflower");
        puts("4) Grind up some Gorak Claws");
        puts("5) Add some water");
        puts("6) Wait a bit");
        puts("7) It's all done!");

        char line[128];
        if (!fgets(line, 128, stdin))
            break;
        int choice = atoi(line);

        switch (choice)
        {
        case 1:
            spot--;
            if (spot < 0)
                spot = 0;
            break;
        case 2:
            spot++;
            if (spot >= sizeof(brew))
                spot = 0;
            break;
        case 3:
            brew[spot] += 0x10;
            break;
        case 4:
            brew[spot] += 0x48;
            break;
        case 5:
            brew[spot] += 0xc;
            break;
        case 6:
            brew[spot] -= 3;
            break;
        case 7:
        {
            puts("OK, let's try out this potion!");
            void* ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            memcpy(ptr, brew, sizeof(brew));
            ((void(*)())ptr)();
        }
        }
    }
}
