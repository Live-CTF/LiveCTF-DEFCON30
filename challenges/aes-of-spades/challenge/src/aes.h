#ifndef AES_H_
#define AES_H_

#include <stdint.h>

#define AES_256_ROUNDS 14
#define AES_192_ROUNDS 12
#define AES_128_ROUNDS 10

// Uncomment this (or compile with -DOPT_8_BIT) to optimise for an 8 bit architecture
// #define AES_OPT_8_BIT

#ifdef AES_OPT_8_BIT
  typedef uint8_t counter;
#else
  typedef unsigned int counter;
#endif

// AES-256

typedef struct aes_256_context_t_ {
  uint8_t round_key[(AES_256_ROUNDS + 1) * 16];
} aes_256_context_t;

void aes_256_init    (aes_256_context_t *context, uint8_t key[32]);
void aes_256_encrypt (aes_256_context_t *context, uint8_t block[16]);
void aes_256_decrypt (aes_256_context_t *context, uint8_t block[16]);

// AES-192

typedef struct aes_192_context_t_ {
  uint8_t round_key[(AES_192_ROUNDS + 1) * 16];
} aes_192_context_t;

void aes_192_init    (aes_192_context_t *context, uint8_t key[24]);
void aes_192_encrypt (aes_192_context_t *context, uint8_t block[16]);
void aes_192_decrypt (aes_192_context_t *context, uint8_t block[16]);

// AES-128

typedef struct aes_128_context_t_ {
  uint8_t round_key[(AES_128_ROUNDS + 1) * 16];
} aes_128_context_t;

void aes_128_init    (aes_128_context_t *context, uint8_t key[16]);
void aes_128_encrypt (aes_128_context_t *context, uint8_t block[16]);
void aes_128_decrypt (aes_128_context_t *context, uint8_t block[16]);

#endif

