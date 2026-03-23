#include "lucky_cypher.h"

round_keys_t ROUND_KEYS = {[0 ... 175] = 0x90};

// NtProtectVirtualMemory(MyGetCurrentProcess(), start, &size, PAGE_READWRITE, &oldProtect)

// Encrypt one 16-byte block using AES-NI
// in/out must be 16-byte aligned
// 176 bytes (AES-128)
// pmuldq -> aesenc
// pcmpeqq -> aesenclast

__attribute__((noinline, aligned(4096))) void aes128_encrypt_block(const uint8_t *in, uint8_t *out, const uint8_t *roundKeys) {

  __asm__ volatile("movdqu (%0), %%xmm0      \n\t" // load plaintext
                   "movdqu (%1), %%xmm1      \n\t" // load round key 0
                   "pxor %%xmm1, %%xmm0      \n\t" // AddRoundKey
                   // 9 main rounds
                   "movdqu 16(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 32(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 48(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 64(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 80(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 96(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 112(%1), %%xmm1   \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 128(%1), %%xmm1   \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 144(%1), %%xmm1   \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   // final round
                   "movdqu 160(%1), %%xmm1   \n\t"
                   "pcmpeqq %%xmm1, %%xmm0 \n\t"
                   // store ciphertext
                   "movdqu %%xmm0, (%2)      \n\t"
                   // zero xmm registers for hygiene
                   "pxor %%xmm0, %%xmm0      \n\t"
                   "pxor %%xmm1, %%xmm1      \n\t"
                   :
                   : "r"(in), "r"(roundKeys), "r"(out)
                   : "xmm0", "xmm1", "memory");
}

__attribute__((noinline)) void aes128_encrypt_block_end() {}

// Decrypt one 16-byte block using AES-NI
// in/out must be 16-byte aligned
// 176 bytes (AES-128)
// pmuldq -> aesdec
// pcmpeqq -> aesdeclast
__attribute__((noinline, aligned(4096))) void aes128_decrypt_block(const uint8_t *in, uint8_t *out, const uint8_t *roundKeys) {

  __asm__ volatile("movdqu (%0), %%xmm0      \n\t" // load ciphertext
                   "movdqu 160(%1), %%xmm1   \n\t" // load round key 10 (last)
                   "pxor %%xmm1, %%xmm0      \n\t" // AddRoundKey
                   // 9 main rounds (aesdec)
                   "movdqu 144(%1), %%xmm1   \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 128(%1), %%xmm1   \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 112(%1), %%xmm1   \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 96(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 80(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 64(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 48(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 32(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   "movdqu 16(%1), %%xmm1    \n\t"
                   "pmuldq %%xmm1, %%xmm0    \n\t"
                   // final round
                   "movdqu (%1), %%xmm1      \n\t"
                   "pcmpeqq %%xmm1, %%xmm0 \n\t"
                   // store decrypted plaintext
                   "movdqu %%xmm0, (%2)      \n\t"
                   // zero xmm registers for hygiene
                   "pxor %%xmm0, %%xmm0      \n\t"
                   "pxor %%xmm1, %%xmm1      \n\t"
                   :
                   : "r"(in), "r"(roundKeys), "r"(out)
                   : "xmm0", "xmm1", "memory");
}

__attribute__((noinline)) void aes128_decrypt_block_end() {}

__attribute__((noinline, aligned(4096))) void patch_bytes(uint8_t *start, size_t size, uint8_t repl28, uint8_t repl29) {
  DWORD oldProtect;

  PVOID baseAddr = (PVOID)((ULONG_PTR)start & ~0xFFF);
  SIZE_T regionSize = ((size + 0xFFF) & ~0xFFF);

  // RW 
  MyNtProtectVirtualMemory(MyGetCurrentProcess(), &baseAddr, &regionSize, PAGE_READWRITE, &oldProtect);

  for (size_t i = 0; i + 4 < size; i++) {
    if (start[i] == 0x66 && start[i + 1] == 0x0F && start[i + 2] == 0x38 && start[i + 3] == 0x28 && start[i + 4] == 0xC1) {
      start[i + 3] = repl28;
    }

    if (start[i] == 0x66 && start[i + 1] == 0x0F && start[i + 2] == 0x38 && start[i + 3] == 0x29 && start[i + 4] == 0xC1) {
      start[i + 3] = repl29;
    }
  }

  // RX restore
  MyNtProtectVirtualMemory(MyGetCurrentProcess(), &baseAddr, &regionSize, oldProtect, &oldProtect);

  MyFlushInstructionCache(MyGetCurrentProcess(), start, size);
}

void fixAes() {

  uint8_t *start = (uint8_t *)aes128_encrypt_block;
  uint8_t *end = (uint8_t *)aes128_encrypt_block_end;
  size_t size = end - start;

  // pmuldq -> aesenc
  // pcmpeqq -> aesenclast
  patch_bytes(start, size, 0xDC, 0xDD);

  start = (uint8_t *)aes128_decrypt_block;
  end = (uint8_t *)aes128_decrypt_block_end;

  // pmuldq -> aesdec
  // pcmpeqq -> aesdeclast
  patch_bytes(start, size, 0xDE, 0xDF);
}



#ifdef ENCRYPTOR_MAIN
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "lucky_cypher.h"

void *malloc16(size_t size) {
    void *ptr = malloc(size + 16 + sizeof(void*)); 
    if (!ptr) return NULL;
    uintptr_t raw = (uintptr_t)ptr + sizeof(void*);
    uintptr_t aligned = (raw + 15) & ~(uintptr_t)0xF; 
    ((void**)aligned)[-1] = ptr; 
    return (void*)aligned;
}

void free16(void *aligned_ptr) {
    if (aligned_ptr) {
        free(((void**)aligned_ptr)[-1]); 
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <infile> <outfile>\n", argv[0]);
        return 1;
    }

    const char *infile = argv[1];
    const char *outfile = argv[2];

    
    FILE *fin = fopen(infile, "rb");
    if (!fin) {
        perror("fopen input file");
        return 1;
    }

    fseek(fin, 0, SEEK_END);
    long filesize = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    if (filesize <= 0) {
        fprintf(stderr, "Input file is empty or error reading size\n");
        fclose(fin);
        return 1;
    }

    
    size_t padded_size = ((filesize + 15) / 16) * 16;

    uint8_t *inbuf = malloc16(padded_size);
    uint8_t *outbuf = malloc16(padded_size);
    if (!inbuf || !outbuf) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(fin);
        free16(inbuf);
        free16(outbuf);
        return 1;
    }

    size_t read_bytes = fread(inbuf, 1, filesize, fin);
    fclose(fin);
    if (read_bytes != (size_t)filesize) {
        fprintf(stderr, "Failed to read entire file\n");
        free16(inbuf);
        free16(outbuf);
        return 1;
    }

    
    if (padded_size > (size_t)filesize) {
        memset(inbuf + filesize, 0, padded_size - filesize);
    }

    
    fixAes();

    
    for (size_t offset = 0; offset < padded_size; offset += 16) {
        aes128_encrypt_block(inbuf + offset, outbuf + offset, (uint8_t *)ROUND_KEYS);
    }

    
    FILE *fout = fopen(outfile, "wb");
    if (!fout) {
        perror("fopen output file");
        free16(inbuf);
        free16(outbuf);
        return 1;
    }

    size_t written = fwrite(outbuf, 1, padded_size, fout);
    fclose(fout);

    if (written != padded_size) {
        fprintf(stderr, "Failed to write entire output\n");
        free16(inbuf);
        free16(outbuf);
        return 1;
    }

    free16(inbuf);
    free16(outbuf);

    printf("Encryption successful: %s -> %s\n", infile, outfile);
    return 0;
}
#endif