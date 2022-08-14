#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "aes.h"

void init(void)
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main()
{
  init();
  mprotect(0x401000, 0x2000, PROT_WRITE|PROT_EXEC|PROT_READ);

  aes_128_context_t aes;

  uint8_t key[16] = {0};
  puts("Key: ");
  fread(key, 16, 1, stdin);

  uint8_t iv[16] = {0};
  puts("IV: ");
  fread(iv, 16, 1, stdin);

  puts("Address: ");
  uint64_t addr = 0;
  scanf("%lu", &addr);

  for (int i = 0; i < 16; i++)
  {
    ((uint8_t*)addr)[i] ^= iv[i];
  }

  aes_128_init(&aes, key);
  aes_128_encrypt(&aes, (uint8_t*)addr);

  for (int i = 0; i < 16; i++)
  {
    printf("%02x", ((uint8_t*)addr)[i]);
  }
  return 0;
}
