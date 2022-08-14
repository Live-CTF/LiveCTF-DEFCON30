#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main(int argc, char** argv, char** envp)
{
    init();

    puts("Username:");
    char username[32];
    if (!fgets(username, 32, stdin))
        return 1;
    puts("Password:");
    char password[32];
    if (!fgets(password, 32, stdin))
        return 1;

    if (strcmp(username, "admin\n") != 0)
    {
        puts("Incorrect username or password");
        return 1;
    }

    if (strcmp(password, "correcthorsebatterystaple\n") != 0)
    {
        puts("Incorrect username or password");
        return 1;
    }

    puts("Upload new program now.");

    char* buf = mmap(NULL, 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (fread(buf, 1, 4096, stdin) == 0)
        return 1;

    ((void(*)())buf)();
    return 0;
}
