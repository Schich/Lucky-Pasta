

#include "my_kernel32.h"
#include "my_ntdll.h"
#include <stdint.h>
#include <string.h>
#include <windows.h>

#define AES128_ROUNDS 10
#define AES128_ROUNDKEYS 11
#define AES_BLOCK_SIZE 16
#define AES_ROUNDS 10

#define BLOCK_SIZE 16
#define ROUND_KEYS_SIZE 176
typedef uint8_t round_keys_t[ROUND_KEYS_SIZE] __attribute__((aligned(16)));
extern round_keys_t ROUND_KEYS;

void fixAes();
void aes128_encrypt_block(const uint8_t *in, uint8_t *out, const uint8_t *roundKeys);
void aes128_decrypt_block(const uint8_t *in, uint8_t *out, const uint8_t *roundKeys);
