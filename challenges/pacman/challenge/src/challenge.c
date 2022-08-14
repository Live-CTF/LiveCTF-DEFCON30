#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <err.h>
#include <linux/limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define sign_ptr(ptr) { \
    void *p = ptr; \
    __asm__ __volatile__("pacdza %0" : "=r"(p) : "r"(p)); \
    ptr = p; \
}
#define auth_ptr(ptr) { \
    void *p = ptr; \
    __asm__ __volatile__("autdza %0" : "=r"(p) : "r"(p)); \
    ptr = p; \
}

bool is_running_safe()
{
    char *unsafe = getenv("PACMAN_UNSAFE");
    if (!unsafe) {
        return true;
    }
    return !strncmp(unsafe, "1", 1);
}

int read_exactly(int fd, void *buf, size_t n)
{
    size_t remains = n;
    while (remains) {
        ssize_t nread = read(fd, buf, remains);
        if (nread < 0) {
            err(1, "read");
        }
        remains -= nread;
        buf += nread;
    }
    return 0;
}

int write_exactly(int fd, void *buf, size_t n)
{
    size_t remains = n;
    while (remains) {
        ssize_t nwrite = write(fd, buf, remains);
        if (nwrite < 0) {
            err(1, "write");
        }
        remains -= nwrite;
        buf += nwrite;
    }
    return 0;
}

size_t getinput(const char *prompt, char **buf)
{
    printf("%s", prompt);

    size_t len = 0;
    if ((len = getline(buf, &len, stdin)) < 0) {
        err(1, "getinput");
    }

    if ((*buf)[len-1] == '\n') {
        (*buf)[len-1] = 0;
        len--;
    }

    return len;
}

size_t read_size_field(int fd)
{
    char len_str[8] = {0};
    read_exactly(fd, len_str, sizeof(len_str) - 1);
    len_str[sizeof(len_str) - 1] = 0;

    size_t len = strtoul(len_str, NULL, 10);
    return len;
}

enum object_type {
    End = '0',
    File = '1',
    Script = '2',
};

typedef struct package_file {
    char *data;
    size_t data_len;
    char path[PATH_MAX];
    size_t path_len;
} package_file;

void file_handler(package_file *file)
{
    int fd = open(file->path, O_CREAT | O_WRONLY, 0644);
    if (fd < 0) {
        err(1, "open(file)");
    }
    auth_ptr(file->data);
    write(fd, file->data, file->data_len);
    close(fd);
}

typedef struct package_script {
    char *script;
} package_script;

void script_handler(package_script *script)
{
    auth_ptr(script->script);
    if (system(script->script) < 0) {
        err(1, "system(script)");
    }
}

typedef struct package_object {
    struct package_object *next;
    enum object_type type;
    union {
        package_file file;
        package_script script;
    };
    void (*handler)(void *);
} package_object;

void write_package_blob(int fd, struct package_object *head)
{
    struct package_object *cur = head;
    while (cur != NULL) {
        dprintf(fd, "%c", cur->type);
        switch (cur->type) {
            case End:
                return;
            case File:
                dprintf(fd, "%07lu", cur->file.path_len);
                write_exactly(fd, cur->file.path, cur->file.path_len);
                dprintf(fd, "%07lu", cur->file.data_len);
                auth_ptr(cur->file.data);
                write_exactly(fd, cur->file.data, cur->file.data_len);
                break;
            case Script:
                auth_ptr(cur->script.script);
                dprintf(fd, "%07lu", strlen(cur->script.script));
                dprintf(fd, "%s", cur->script.script);
                break;
            default:
                fprintf(stderr, "Corrupt object type int = %d\n", cur->type);
                exit(1);
        }
        auth_ptr(cur->next);
        cur = cur->next;
    }
}

void build_package()
{
    struct package_object *head = NULL;
    struct package_object *cur  = NULL;

    bool end = false;

    while (!end) {
        puts("Object type:");
        puts("0 = EOF");
        puts("1 = File");
        puts("2 = Run Script");

        struct package_object *o = calloc(1, sizeof(struct package_object));
        if (!o) {
            err(1, "malloc(package_object)");
        }

        char *type = NULL;
        size_t type_len = getinput("? ", &type);

        if (!type || strlen(type) != 1) {
            fprintf(stderr, "Invalid object type input %s\n", type);
        }

        o->type = *type;

        switch (o->type) {
            case End: {
                end = true;
                break;
            }
            case File: {
                char *path = NULL;
                size_t path_len = getinput("Path? ", &path);

                char *data = NULL;
                size_t data_len = getinput("Data? ", &data);

                o->handler = file_handler;
                o->file.data = data;
                o->file.data_len = data_len;
                memcpy(o->file.path, path, path_len);
                o->file.path_len = path_len;

                sign_ptr(o->file.data);

                break;
            }
            case Script: {
                char *data = NULL;
                size_t data_len = getinput("Script? ", &data);

                o->handler = script_handler;
                o->script = (struct package_script){
                    .script = data,
                };

                sign_ptr(o->script.script);

                break;
            }
            default: {
                fprintf(stderr, "Bad object type int = %d\n", o->type);
                exit(1);
            }
        }

        sign_ptr(o->handler);

        if (!head) {
            head = o;
        } else {
            cur->next = o;
            sign_ptr(cur->next);
        }
        cur = o;
    }

    printf("Package blob: ");
    write_package_blob(STDOUT_FILENO, head);
}

/*
 * Installing
 */

struct package_object *parse_package_blob(int fd)
{
    struct package_object *head = NULL;
    struct package_object *cur  = NULL;

    bool end = false;

    while (!end) {
        struct package_object *o = calloc(1, sizeof(struct package_object));
        if (!o) {
            err(1, "malloc(package_object)");
        }
        read_exactly(fd, &o->type, 1);
        printf("Parsing object of type %c\n", o->type);

        switch (o->type) {
            case End: {
                end = true;
                break;
            }
            case File: {
                size_t path_len = read_size_field(fd);
                char *path = malloc(path_len);
                if (!path) {
                    err(1, "malloc(path)");
                }
                read_exactly(fd, path, path_len);

                if (is_running_safe() && strstr(path, "/")) {
                    fprintf(stderr, "Subdirectories not allowed in safe mode\n");
                    exit(1);
                }

                size_t data_len = read_size_field(fd);
                char *data = malloc(data_len);
                if (!data) {
                    err(1, "malloc(data)");
                }
                read_exactly(fd, data, data_len);

                o->handler = file_handler;
                sign_ptr(o->handler);

                o->file.data = data;
                o->file.data_len = data_len;
                memcpy(o->file.path, path, path_len);
                o->file.path_len = path_len;
                sign_ptr(o->file.data);

                break;
            }
            case Script: {
                if (is_running_safe()) {
                    fprintf(stderr, "Scripts not allowed unless PACMAN_UNSAFE=1\n");
                    exit(1);
                }

                size_t data_len = read_size_field(fd);
                char *data = malloc(data_len);
                if (!data) {
                    err(1, "malloc(data)");
                }
                read_exactly(fd, data, data_len);

                o->handler = script_handler;
                sign_ptr(o->handler);

                o->script = (struct package_script){
                    .script = data,
                };
                sign_ptr(o->script.script);

                break;
            }
            default: {
                fprintf(stderr, "Bad object type int = %d\n", o->type);
                exit(1);
            }
        }

        if (!head) {
            head = o;
        } else {
            cur->next = o;
            sign_ptr(cur->next);
        }
        cur = o;
    }

    return head;
}

void install_package()
{
    struct package_object *cur = parse_package_blob(STDIN_FILENO);

    while (cur != NULL) {
        auth_ptr(cur->handler);

        printf("Evaluating object of type %c...\n", cur->type);
        switch (cur->type) {
            case End:
                exit(0);
            case File:
                cur->handler(&cur->file);
                break;
            case Script:
                cur->handler(&cur->script);
                break;
            default:
                fprintf(stderr, "Corrupt object type int = %d\n", cur->type);
                exit(1);
        }

        auth_ptr(cur->next);
        cur = cur->next;
    }
}

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main()
{
    init();

    puts("This is PACman v0.1");
    puts("");

    while (true) {
        puts("What do you want to do?");
        puts("");
        puts("0) Exit");
        puts("1) Build package");
        puts("2) Install package");
        printf("? ");

        char choice[2] = {'0'};
        read(STDIN_FILENO, &choice, sizeof(choice));
        switch (choice[0]) {
            case '0':
            case '\n':
                exit(0);
            case '1':
                build_package();
                break;
            case '2':
                install_package();
                break;
            default:
                puts("Invalid choice");
        }

        puts("");
    }

    return 0;
}
