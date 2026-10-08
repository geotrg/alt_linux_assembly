/*
 * rsa_compare.c — runs the SAME message through both the "dumb" and
 * the "optimized" RSA modular exponentiation, back to back, so their
 * timings can be compared directly in one run instead of eyeballing
 * two separate program outputs.
 *
 * Build:  gcc -O2 -Wall -Wextra -o rsa_compare rsa_compare.c
 * Run:    ./rsa_compare
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#define RSA_N 1950499UL
#define RSA_E 65537UL
#define RSA_D 235473UL

/* --- "Dumb" version: O(exp) repeated-multiplication loop --- */
static unsigned long modexp_naive(unsigned long base, unsigned long exp, unsigned long mod) {
    unsigned long result = 1UL % mod;
    base = base % mod;
    for (unsigned long i = 0; i < exp; i++) {
        result = (result * base) % mod;
    }
    return result;
}
unsigned long encrypt_naive(unsigned long block, unsigned long e, unsigned long n) { return modexp_naive(block, e, n); }
unsigned long decrypt_naive(unsigned long block, unsigned long d, unsigned long n) { return modexp_naive(block, d, n); }

/* --- Optimized version: O(log2 exp) square-and-multiply --- */
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
unsigned long encrypt_fast(unsigned long block, unsigned long e, unsigned long n) { return modexp_fast(block, e, n); }
unsigned long decrypt_fast(unsigned long block, unsigned long d, unsigned long n) { return modexp_fast(block, d, n); }

/* Run one full encrypt+decrypt pass over `plaintext`, timing each phase. */
static void run_pass(
    const char *label,
    unsigned long (*enc)(unsigned long, unsigned long, unsigned long),
    unsigned long (*dec)(unsigned long, unsigned long, unsigned long),
    const char *plaintext, size_t len,
    double *out_encrypt_s, double *out_decrypt_s, int *out_ok
) {
    unsigned long cipher[1024];
    char recovered[1024];

    clock_t t0 = clock();
    for (size_t i = 0; i < len; i++) {
        cipher[i] = enc((unsigned char)plaintext[i], RSA_E, RSA_N);
    }
    clock_t t1 = clock();
    for (size_t i = 0; i < len; i++) {
        recovered[i] = (char)dec(cipher[i], RSA_D, RSA_N);
    }
    clock_t t2 = clock();
    recovered[len] = '\0';

    *out_encrypt_s = (double)(t1 - t0) / CLOCKS_PER_SEC;
    *out_decrypt_s = (double)(t2 - t1) / CLOCKS_PER_SEC;
    *out_ok = (strncmp(plaintext, recovered, len) == 0);

    printf("--- %s ---\n", label);
    printf("Encrypted (first few blocks): ");
    for (size_t i = 0; i < len && i < 6; i++) printf("%lu ", cipher[i]);
    if (len > 6) printf("...");
    printf("\n");
    printf("Decrypted text: %s\n", recovered);
    printf("Round-trip: %s\n", *out_ok ? "OK" : "MISMATCH!");
    printf("Encrypt time: %.6f s\n", *out_encrypt_s);
    printf("Decrypt time: %.6f s\n", *out_decrypt_s);
    printf("\n");
}

int main(void) {
    char plaintext[1024];

    printf("=== RSA performance comparison: dumb vs optimized modular exponentiation ===\n");
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
    printf("\n");

    double dumb_enc, dumb_dec, fast_enc, fast_dec;
    int dumb_ok, fast_ok;

    run_pass("DUMB (naive loop)", encrypt_naive, decrypt_naive, plaintext, len, &dumb_enc, &dumb_dec, &dumb_ok);
    run_pass("OPTIMIZED (square-and-multiply)", encrypt_fast, decrypt_fast, plaintext, len, &fast_enc, &fast_dec, &fast_ok);

    double dumb_total = dumb_enc + dumb_dec;
    double fast_total = fast_enc + fast_dec;

    printf("=== Summary (%zu characters) ===\n", len);
    printf("%-12s %12s %12s %12s\n", "Version", "Encrypt(s)", "Decrypt(s)", "Total(s)");
    printf("%-12s %12.6f %12.6f %12.6f\n", "Dumb", dumb_enc, dumb_dec, dumb_total);
    printf("%-12s %12.6f %12.6f %12.6f\n", "Optimized", fast_enc, fast_dec, fast_total);

    if (fast_total > 0.0) {
        printf("\nOptimized was %.1fx faster overall.\n", dumb_total / fast_total);
    } else {
        printf("\nOptimized total time too small to measure a meaningful ratio (try a longer input).\n");
    }

    if (!dumb_ok || !fast_ok) {
        printf("\nWARNING: at least one round-trip did not match the original input!\n");
    }

    return 0;
}
