#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#define N 256   // 2^8

#define AREA_LEN 0x1000*0x1000
#define STACK_LEN 0x1000

void swap(unsigned char *a, unsigned char *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int KSA(char *key, unsigned char *S, size_t len) {

    int j = 0;

    for(int i = 0; i < N; i++)
        S[i] = i;

    for(int i = 0; i < N; i++) {
        j = (j + S[i] + key[i % len]) % N;

        swap(&S[i], &S[j]);
    }

    return 0;
}

int PRGA(unsigned char *S, char *plaintext, size_t len) {

    int i = 0;
    int j = 0;

    for(size_t n = 0; n < len; n++) {
        i = (i + 1) % N;
        j = (j + S[i]) % N;

        swap(&S[i], &S[j]);
        int rnd = S[(S[i] + S[j]) % N];

        plaintext[n] ^= rnd;

    }

    return 0;
}


void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main(int argc, char** argv, char** envp)
{
    init();

    void *area = mmap(NULL, AREA_LEN, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    memset(area, 0, AREA_LEN);

    printf("The toolbox: %p\n", area);
    
    printf("Please provide a key: ");
    char key[16];
    fgets(key, sizeof(key), stdin);
    unsigned char S[N];
    KSA(key, S, sizeof(key));
    PRGA(S, area, AREA_LEN);

    for(size_t i = 0; i < AREA_LEN; i += 0x10) {
        ((unsigned char*)area)[i] = 0xC3;
    }

    mprotect(area, AREA_LEN, PROT_READ | PROT_EXEC);

    void *stack = mmap(NULL, STACK_LEN, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    printf("Give me some of that sweet ROP: ");
    fread(stack, sizeof(unsigned char), STACK_LEN, stdin);

    __asm__ (
        "mov %0, %%rax\n"
        "mov %%rax, %%rsp\n"
        "ret"
        :
        : "r" (stack)
    );
    
    return 0;
}
