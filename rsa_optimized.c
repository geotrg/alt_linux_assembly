/*
 * rsa_optimized.c — RSA, optimized version.
 *
 * Same keys, same block-per-character scheme as rsa_dumb.c — the
 * only thing that changes is HOW base^exp mod n gets computed:
 * square-and-multiply (a.k.a. fast/binary exponentiation), which
 * only needs O(log2(exp)) multiplications instead of rsa_dumb.c's
 * O(exp). For d = 235473 (~18 bits), that is about 18 multiplications
 * here versus 235473 there.
 *
 * Build:  gcc -O2 -Wall -Wextra -o rsa_optimized rsa_optimized.c
 * Run:    ./rsa_optimized
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

/* Same demo keypair as rsa_dumb.c — see that file for how it was derived. */
#define RSA_N 1950499UL
#define RSA_E 65537UL
#define RSA_D 235473UL

/*
 * modexp_fast — compute (base^exp) mod mod via square-and-multiply.
 * Walk the bits of exp from least to most significant: square `base`
 * every step, and fold it into the running result whenever the
 * current bit is 1. O(log2(exp)) multiply+mod operations total —
 * this is the function the whole "optimized" label refers to.
 *
 * mod here (1950499) is small enough that base*base can't overflow
 * a 64-bit unsigned long, so plain unsigned long arithmetic is safe
 * without needing __int128 or a separate mulmod helper.
 */
static unsigned long modexp_fast(unsigned long base, unsigned long exp, unsigned long mod) {
    unsigned long result = 1UL % mod;
    base = base % mod;
    while (exp > 0) {
        if (exp & 1UL) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

/* encrypt_fast — RSA-encrypt one block (0..n-1) with the public key (e, n). */
unsigned long encrypt_fast(unsigned long block, unsigned long e, unsigned long n) {
    return modexp_fast(block, e, n);
}

/* decrypt_fast — RSA-decrypt one block with the private key (d, n). */
unsigned long decrypt_fast(unsigned long block, unsigned long d, unsigned long n) {
    return modexp_fast(block, d, n);
}

int main(void) {
    char plaintext[1024];

    printf("=== RSA (optimized / square-and-multiply modular exponentiation) ===\n");
    printf("n = %lu, e = %lu, d = %lu\n\n", RSA_N, RSA_E, RSA_D);

    printf("Enter text to encrypt: ");
    if (!fgets(plaintext, sizeof(plaintext), stdin)) {
        fprintf(stderr, "Failed to read input.\n");
        return 1;
    }
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
        cipher[i] = encrypt_fast((unsigned char)plaintext[i], RSA_E, RSA_N);
    }
    clock_t t1 = clock();
    for (size_t i = 0; i < len; i++) {
        recovered[i] = (char)decrypt_fast(cipher[i], RSA_D, RSA_N);
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
