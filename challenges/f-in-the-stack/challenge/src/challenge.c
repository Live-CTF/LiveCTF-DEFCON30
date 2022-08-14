#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILES 8
#define MAX_FILENAME_LEN 64

struct file_contents {
    char* name;
    uint8_t* contents;
    uint32_t len;
};


struct file_contents files[MAX_FILES] = { 0 };

uint8_t num_files = 0;

const char* MENU = "\n1. Load a file\n"
"2. Print loaded file\n"
"3. Unload last file\n"
"4. Extremely convenient call rax\n";


uint8_t get_uint_input(uint64_t* out)
{
    putchar('>');
    char input[32] = {0};
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        puts("NULL");
        return 0;
    }
    char* endptr;
    uint64_t result = strtoull(input, &endptr, 0);
    if (endptr == input)
        return 0;
    *out = result;
    return 1;
}


int valid_path(const char* fn)
{
    for (int i = 0; i < strlen(fn); i++)
    {
        char c = fn[i];
        if (
            (c >= '-' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') || (c == '_')
            )
        {
            continue;
        }
        return 0;
    }
    return 1;
}


void pop_file()
{
    if (num_files == 0)
    {
        puts("No file to remove");
        return;
    }

    struct file_contents latest_file = files[num_files - 1];
    if (latest_file.name != NULL)
        free(latest_file.name);
    if (latest_file.contents != NULL)
        free(latest_file.contents);
    num_files--;
}


void read_file()
{
    char fn[MAX_FILENAME_LEN] = {0};
    puts("File name?");
    putchar('>');
    if (fgets(fn, sizeof(fn), stdin) == NULL)
        return;

    fn[strcspn(fn, "\n")] = 0;

    if (!valid_path(fn))
    {
        puts("Bad filename");
        return;
    }

    if (num_files >= MAX_FILES)
    {
        puts("Too many files");
        return;
    }

    // BUG: Get file size from lstat instead of fstat
    struct stat st;
    if (lstat(fn, &st) != 0)
    {
        puts("fstat failed");
        return;
    }

    // yes, there's a toctou race here
    FILE* fp = fopen(fn, "r");
    if (fp == NULL)
    {
        puts("fopen failed");
        goto cleanup;
    }

    if ((st.st_mode & __S_IFREG) == 0)
    {
        puts("Invalid file type");
        goto cleanup;
    }

    int64_t fs = st.st_size;
    // TODO: do we need more checks here
    if (fs <= 0 || fs > UINT32_MAX)
    {
        puts("Invalid file size");
        goto cleanup;
    }


    uint8_t* buf = malloc(fs);
    if (buf == NULL)
    {
        puts("Failed to allocate");
        goto cleanup;
    }

    // BUG: unchecked fread leads to leak on early EOF
    // BUG: reading symlink to file with name larger than contents leads to leak
    fread(buf, 1, fs, fp);

    struct file_contents new_file;

    new_file.name = strcpy(malloc(MAX_FILENAME_LEN), fn);
    new_file.contents = buf;
    new_file.len = fs;

    files[num_files] = new_file;
    num_files++;

    cleanup:
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    return;
}


void print_file()
{
    uint64_t choice;
    puts("Index?");
    if (!get_uint_input(&choice))
    {
        puts("Invalid input");
        return;
    }

    if (choice >= num_files)
    {
        puts("Index too large");
        return;
    }

    struct file_contents f = files[choice];
    fwrite(f.contents, 1, f.len, stdout);
}


void arb_call()
{
    uint64_t out = 0;
    if (!get_uint_input(&out))
        return;
    asm(
        ".intel_syntax noprefix;"
        "pop rax;"
        "xor rdi, rdi;"
        "xor rsi, rsi;"
        "call rax;"
    );
}


int main()
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    while (1)
    {
        if (num_files)
        {
            puts("Your files:");
            for (uint8_t i = 0; i < num_files; i++)
            {
                struct file_contents f = files[i];
                printf("  %u: %s (%u bytes)\n", i, f.name, f.len);
            }
        }

        puts(MENU);

        uint64_t choice;
        if (!get_uint_input(&choice))
        {
            puts("Invalid input");
            return 0;
        }
        switch (choice)
        {
            case 1:
                read_file();
                break;
            case 2:
                print_file();
                break;
            case 3:
                pop_file();
                break;
            case 4:
                arb_call();
                break;
            default:
                puts("Invalid choice");
                continue;
        }
    }

    return 0;
}
