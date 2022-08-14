#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

union {
    int64_t num;
    char* text;
} value = {0};

char textbuf[256] = {0};

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int64_t num(void) {
    char line[256];
    if (!fgets(line, 256, stdin))
        exit(0);
    return strtoll(line, NULL, 0);
}

void show_value(void) {
    puts("Type of value:");
    puts("1) Number");
    puts("2) Text");
    int64_t choice = num();
    switch (choice)
    {
    case 1:
        printf("Value is:\n%" PRId64 "\n", value.num);
        break;
    case 2:
        puts("Value is:");
        printf("%s\n", value.text);
        break;
    default:
        break;
    }
}

void edit_value(void) {
    puts("Type of value:");
    puts("1) Number");
    puts("2) Text");
    int64_t choice = num();
    size_t len;
    switch (choice)
    {
    case 1:
        puts("New value:");
        value.num = num();
        break;
    case 2:
        puts("New value:");
        if (!value.text)
            value.text = textbuf;
        if (!fgets(value.text, 256, stdin))
            exit(0);
        len = strlen(value.text);
        if (len >= 1 && value.text[len - 1] == '\n')
            value.text[len - 1] = 0;
        break;
    default:
        break;
    }
}

int main(int argc, char** argv, char** envp)
{
    init();

    while (1)
    {
        puts("Menu:");
        puts("1) Show value");
        puts("2) Edit value\n");

        int choice = num();
        switch (choice)
        {
        case 1:
            show_value();
            break;
        case 2:
            edit_value();
            break;
        default:
            return 0;
        }
    }
}
