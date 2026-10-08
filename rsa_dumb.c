/*
 * rsa_dumb.c — RSA, "dumb" / unoptimized version.
 *
 * Modular exponentiation is done the naive way: to compute
 * base^exp mod n, multiply by base, exponent times, in a straight
 * loop. That is O(exp) multiplications — for a real RSA private
 * exponent d (hundreds of thousands here, billions in real-world
 * RSA-2048 keys), this is catastrophically slow. It exists in this
 * assignment purely as the "before" picture, to be compared against
 * rsa_optimized.c's square-and-multiply approach.
 *
 * Build:  gcc -O0 -Wall -Wextra -o rsa_dumb rsa_dumb.c
 * Run:    ./rsa_dumb
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

/* --------------------------------------------------------------
 * Fixed demo keypair (small enough for plain 64-bit arithmetic,
 * large enough that the naive loop below is clearly, measurably
 * slower than the optimized version for the same key).
 *
 *   p = 809, q = 2411
 *   n = p*q            = 1950499
 *   phi = (p-1)*(q-1)  = 1947280
 *   e (public)         = 65537
 *   d (private)        = 235473   (= e^-1 mod phi)
 *
 * Each character of the message is encrypted as its own block
 * (0-255), which is always < n, so no block-splitting logic is
 * needed for this demo.
 * -------------------------------------------------------------- */
#define RSA_N 1950499UL
#define RSA_E 65537UL
#define RSA_D 235473UL

/*
 * modexp_naive — compute (base^exp) mod mod, the "dumb" way: a
 * straight loop that multiplies in `exp` times. O(exp) multiply+mod
 * operations. This is the function the whole "dumb" label refers
 * to; everything else in this file is just plumbing around it.
 */
static unsigned long modexp_naive(unsigned long base, unsigned long exp, unsigned long mod) {
    unsigned long result = 1UL % mod;
    base = base % mod;
    for (unsigned long i = 0; i < exp; i++) {
        result = (result * base) % mod;
    }
    return result;
}

/* encrypt_naive — RSA-encrypt one block (0..n-1) with the public key (e, n). */
unsigned long encrypt_naive(unsigned long block, unsigned long e, unsigned long n) {
    return modexp_naive(block, e, n);
}

/* decrypt_naive — RSA-decrypt one block with the private key (d, n). */
unsigned long decrypt_naive(unsigned long block, unsigned long d, unsigned long n) {
    return modexp_naive(block, d, n);
}

int main(void) {
    char plaintext[1024];

    printf("=== RSA (dumb / unoptimized modular exponentiation) ===\n");
    printf("n = %lu, e = %lu, d = %lu\n\n", RSA_N, RSA_E, RSA_D);

    printf("Enter text to encrypt: ");
    if (!fgets(plaintext, sizeof(plaintext), stdin)) {
        fprintf(stderr, "Failed to read input.\n");
        return 1;
    }
    /* Strip the trailing newline fgets() leaves in, if present. */
    size_t len = strlen(plaintext);
    if (len > 0 && plaintext[len - 1] == '\n') {
        plaintext[len - 1] = '\0';
        len--;
    }
    if (len == 0) {
        printf("(empty input, nothing to do)\n");
        return 0;
    }

    unsigned long cipher[1024];
    char recovered[1024];

    clock_t t0 = clock();
    for (size_t i = 0; i < len; i++) {
        cipher[i] = encrypt_naive((unsigned char)plaintext[i], RSA_E, RSA_N);
    }
    clock_t t1 = clock();
    for (size_t i = 0; i < len; i++) {
        recovered[i] = (char)decrypt_naive(cipher[i], RSA_D, RSA_N);
    }
    clock_t t2 = clock();
    recovered[len] = '\0';

    double encrypt_s = (double)(t1 - t0) / CLOCKS_PER_SEC;
    double decrypt_s = (double)(t2 - t1) / CLOCKS_PER_SEC;

    printf("\nEncrypted (block values):\n");
    for (size_t i = 0; i < len; i++) {
        printf("%lu ", cipher[i]);
    }
    printf("\n");

    printf("\nDecrypted text: %s\n", recovered);
    printf("Round-trip %s\n", strcmp(plaintext, recovered) == 0 ? "OK" : "MISMATCH!");

    printf("\nEncrypt time: %.6f s\n", encrypt_s);
    printf("Decrypt time: %.6f s\n", decrypt_s);
    printf("Total time:   %.6f s\n", encrypt_s + decrypt_s);

    return 0;
}
