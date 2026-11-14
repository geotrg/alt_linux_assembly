
#include <stdio.h>
#include <string.h>
#include <time.h>

/* =====================================================
   RSA KEY PARAMETERS
   ===================================================== */

#define RSA_P 809UL
#define RSA_Q 2411UL
#define RSA_N 1950499UL
#define RSA_E 65537UL
#define RSA_D 235473UL

/* =====================================================
   RESULT STRUCTURE
   ===================================================== */

typedef struct
{
    double encryption_time;
    double decryption_time;
    double total_time;
    int correct;
} BenchmarkResult;

/* =====================================================
   MATHEMATICAL HELPER FUNCTIONS
   ===================================================== */

unsigned long gcd_ul(unsigned long a, unsigned long b)
{
    while (b != 0UL)
    {
        unsigned long temp = b;
        b = a % b;
        a = temp;
    }

    return a;
}

int is_prime(unsigned long n)
{
    if (n < 2UL)
        return 0;

    if (n == 2UL)
        return 1;

    if (n % 2UL == 0UL)
        return 0;

    for (unsigned long i = 3UL; i <= n / i; i += 2UL)
    {
        if (n % i == 0UL)
            return 0;
    }

    return 1;
}

void print_check(const char *description, int result)
{
    printf("[%s] %s\n",
           result ? "PASS" : "FAIL",
           description);
}

/* =====================================================
   RSA KEY VERIFICATION
   ===================================================== */

int verify_rsa_key(void)
{
    unsigned long n;
    unsigned long phi;
    int all_ok = 1;

    printf("\n====================================\n");
    printf("        RSA KEY VERIFICATION\n");
    printf("====================================\n");

    int p_ok = is_prime(RSA_P);
    int q_ok = is_prime(RSA_Q);

    print_check("P is prime", p_ok);
    print_check("Q is prime", q_ok);

    all_ok &= p_ok;
    all_ok &= q_ok;

    int different = (RSA_P != RSA_Q);

    print_check("P and Q are different", different);
    all_ok &= different;

    n = RSA_P * RSA_Q;

    printf("Calculated n = %lu\n", n);

    int n_ok = (n == RSA_N);

    print_check("n = p * q", n_ok);
    all_ok &= n_ok;

    phi = (RSA_P - 1UL) * (RSA_Q - 1UL);

    printf("Calculated phi(n) = %lu\n", phi);

    int e_range_ok = (RSA_E > 1UL && RSA_E < phi);

    print_check("1 < e < phi(n)", e_range_ok);
    all_ok &= e_range_ok;

    int gcd_ok = (gcd_ul(RSA_E, phi) == 1UL);

    print_check("gcd(e, phi(n)) = 1", gcd_ok);
    all_ok &= gcd_ok;

    unsigned long long ed =
        (unsigned long long)RSA_E * RSA_D;

    int d_ok = ((ed % phi) == 1ULL);

    print_check("(e * d) mod phi(n) = 1", d_ok);
    all_ok &= d_ok;

    printf("------------------------------------\n");

    if (all_ok)
        printf("RSA KEY IS VALID\n");
    else
        printf("RSA KEY VERIFICATION FAILED\n");

    printf("====================================\n");

    return all_ok;
}

/* =====================================================
   NAIVE MODULAR EXPONENTIATION
   O(exp)
   ===================================================== */

unsigned long modexp_naive(
    unsigned long base,
    unsigned long exp,
    unsigned long mod)
{
    /* Invalid modulus: mod == 0 would divide by zero, and
       mod > 2^32 - 1 could overflow the 64-bit product.
       Returns 0 for invalid input. */
    if (mod == 0UL || mod > 0xFFFFFFFFUL)
        return 0UL;

    /* 1 % mod handles mod == 1 correctly (any value mod 1 is 0),
       including the exp == 0 case */
    unsigned long result = 1UL % mod;

    base %= mod;

    for (unsigned long i = 0UL; i < exp; i++)
    {
        result = (unsigned long)(
            ((unsigned long long)result * base) % mod
        );
    }

    return result;
}

/* =====================================================
   OPTIMIZED MODULAR EXPONENTIATION
   SQUARE-AND-MULTIPLY
   O(log exp)
   ===================================================== */

unsigned long modexp_fast(
    unsigned long base,
    unsigned long exp,
    unsigned long mod)
{
    /* Invalid modulus: mod == 0 would divide by zero, and
       mod > 2^32 - 1 could overflow the 64-bit product.
       Returns 0 for invalid input. */
    if (mod == 0UL || mod > 0xFFFFFFFFUL)
        return 0UL;

    /* 1 % mod handles mod == 1 correctly (any value mod 1 is 0),
       including the exp == 0 case */
    unsigned long result = 1UL % mod;

    base %= mod;

    while (exp > 0UL)
    {
        if (exp & 1UL)
        {
            result = (unsigned long)(
                ((unsigned long long)result * base) % mod
            );
        }

        base = (unsigned long)(
            ((unsigned long long)base * base) % mod
        );

        exp >>= 1;
    }

    return result;
}

/* =====================================================
   RSA ENCRYPTION AND DECRYPTION
   ===================================================== */

unsigned long encrypt_naive(unsigned long message)
{
    return modexp_naive(message, RSA_E, RSA_N);
}

unsigned long decrypt_naive(unsigned long ciphertext)
{
    return modexp_naive(ciphertext, RSA_D, RSA_N);
}

unsigned long encrypt_fast(unsigned long message)
{
    return modexp_fast(message, RSA_E, RSA_N);
}

unsigned long decrypt_fast(unsigned long ciphertext)
{
    return modexp_fast(ciphertext, RSA_D, RSA_N);
}

/* =====================================================
   EDGE CASE TESTS
   ===================================================== */

int verify_edge_cases(void)
{
    int all_ok = 1;

    printf("\n====================================\n");
    printf("         EDGE CASE TESTS\n");
    printf("====================================\n");

    struct { unsigned long b, e, m, expect; const char *name; } t[] =
    {
        {5UL, 0UL, 13UL, 1UL, "x^0 mod 13 = 1"},
        {0UL, 5UL, 13UL, 0UL, "0^5 mod 13 = 0"},
        {0UL, 0UL, 13UL, 1UL, "0^0 mod 13 = 1 (convention)"},
        {7UL, 5UL, 1UL,  0UL, "x^5 mod 1 = 0"},
        {7UL, 0UL, 1UL,  0UL, "x^0 mod 1 = 0"},
        {7UL, 5UL, 0UL,  0UL, "mod = 0 rejected (returns 0)"},
        {7UL, 5UL, 0x100000000UL, 0UL, "mod > 2^32-1 rejected"},
        {13UL, 1UL, 13UL, 0UL, "x^1 mod x = 0"},
        {2UL, 10UL, 1000UL, 24UL, "2^10 mod 1000 = 24"}
    };

    int count = sizeof(t) / sizeof(t[0]);

    for (int i = 0; i < count; i++)
    {
        int ok = (modexp_naive(t[i].b, t[i].e, t[i].m) == t[i].expect) &&
                 (modexp_fast(t[i].b, t[i].e, t[i].m) == t[i].expect);

        print_check(t[i].name, ok);
        all_ok &= ok;
    }

    printf("\nEdge case tests: %s\n", all_ok ? "PASSED" : "FAILED");

    return all_ok;
}

/* =====================================================
   FUNCTIONAL TESTS
   ===================================================== */

int verify_round_trip_functions(void)
{
    unsigned long test_messages[] =
    {
        0UL, 1UL, 2UL, 42UL, 65UL, 255UL, 1024UL
    };

    int count = sizeof(test_messages) /
                sizeof(test_messages[0]);

    int all_ok = 1;

    printf("\n====================================\n");
    printf("       RSA FUNCTIONAL TESTS\n");
    printf("====================================\n");

    for (int i = 0; i < count; i++)
    {
        unsigned long message = test_messages[i];

        unsigned long encrypted_naive =
            encrypt_naive(message);

        unsigned long decrypted_naive =
            decrypt_naive(encrypted_naive);

        unsigned long encrypted_fast =
            encrypt_fast(message);

        unsigned long decrypted_fast =
            decrypt_fast(encrypted_fast);

        int naive_ok = (decrypted_naive == message);
        int fast_ok = (decrypted_fast == message);
        int same = (encrypted_naive == encrypted_fast);

        printf("\nMessage: %lu\n", message);

        print_check("Naive round trip", naive_ok);
        print_check("Fast round trip", fast_ok);
        print_check("Ciphertexts match", same);

        all_ok &= naive_ok;
        all_ok &= fast_ok;
        all_ok &= same;
    }

    printf("\nFunctional tests: %s\n",
           all_ok ? "PASSED" : "FAILED");

    return all_ok;
}

/* =====================================================
   BENCHMARK FUNCTION
   ===================================================== */

BenchmarkResult run_benchmark(
    const char *text,
    int optimized)
{
    BenchmarkResult result = {0.0, 0.0, 0.0, 1};

    unsigned long ciphertext[1024];
    unsigned long recovered[1024];

    size_t length = strlen(text);

    if (length > 1024)
        length = 1024;

    clock_t start, end;

    /* ENCRYPTION */

    start = clock();

    for (size_t i = 0; i < length; i++)
    {
        unsigned long message =
            (unsigned char)text[i];

        if (optimized)
            ciphertext[i] = encrypt_fast(message);
        else
            ciphertext[i] = encrypt_naive(message);
    }

    end = clock();

    result.encryption_time =
        (double)(end - start) / CLOCKS_PER_SEC;

    /* DECRYPTION */

    start = clock();

    for (size_t i = 0; i < length; i++)
    {
        if (optimized)
            recovered[i] = decrypt_fast(ciphertext[i]);
        else
            recovered[i] = decrypt_naive(ciphertext[i]);
    }

    end = clock();

    result.decryption_time =
        (double)(end - start) / CLOCKS_PER_SEC;

    result.total_time =
        result.encryption_time + result.decryption_time;

    /* VERIFY RECOVERED PLAINTEXT */

    for (size_t i = 0; i < length; i++)
    {
        if (recovered[i] != (unsigned char)text[i])
        {
            result.correct = 0;
            break;
        }
    }

    return result;
}

/* =====================================================
   DISPLAY INDIVIDUAL RESULTS
   ===================================================== */

void display_result(
    const char *name,
    BenchmarkResult result)
{
    printf("\n%s\n", name);

    printf("Encryption time: %.6f seconds\n",
           result.encryption_time);

    printf("Decryption time: %.6f seconds\n",
           result.decryption_time);

    printf("Total time:      %.6f seconds\n",
           result.total_time);

    printf("Round-trip:      %s\n",
           result.correct ? "SUCCESS" : "FAILED");
}

/* =====================================================
   CALCULATE SPEEDUP
   ===================================================== */

void print_speedup(const char *operation,
                   double naive_time,
                   double fast_time)
{
    printf("%-20s", operation);

    if (fast_time > 0.0)
    {
        printf("%12.6f %12.6f %14.2f x\n",
               naive_time,
               fast_time,
               naive_time / fast_time);
    }
    else if (naive_time > 0.0)
    {
        printf("%12.6f %12.6f %14s\n",
               naive_time,
               fast_time,
               "Below timer resolution");
    }
    else
    {
        printf("%12.6f %12.6f %14s\n",
               naive_time,
               fast_time,
               "Not measurable");
    }
}

/* =====================================================
   DISPLAY COMPARISON TABLE
   ===================================================== */

void display_comparison(
    BenchmarkResult naive,
    BenchmarkResult fast)
{
    printf("\n\n");
    printf("===============================================================\n");
    printf("                 RSA PERFORMANCE COMPARISON\n");
    printf("===============================================================\n");

    printf("%-20s %12s %12s %14s\n",
           "Operation",
           "Naive (s)",
           "Fast (s)",
           "Speedup");

    printf("---------------------------------------------------------------\n");

    print_speedup(
        "Encryption",
        naive.encryption_time,
        fast.encryption_time
    );

    print_speedup(
        "Decryption",
        naive.decryption_time,
        fast.decryption_time
    );

    print_speedup(
        "Total",
        naive.total_time,
        fast.total_time
    );

    printf("===============================================================\n");

    printf("\nFINAL RESULT:\n");

    if (!naive.correct || !fast.correct)
    {
        printf("ERROR: At least one implementation failed.\n");
        return;
    }

    if (fast.total_time > 0.0)
    {
        double total_speedup =
            naive.total_time / fast.total_time;

        printf("Optimized RSA is %.2f times faster overall.\n",
               total_speedup);
    }
    else
    {
        printf("Optimized execution time is below timer resolution.\n");
        printf("Use a longer plaintext for a more reliable measurement.\n");
    }

    if (fast.encryption_time > 0.0)
    {
        printf("Encryption speedup: %.2f times.\n",
               naive.encryption_time /
               fast.encryption_time);
    }

    if (fast.decryption_time > 0.0)
    {
        printf("Decryption speedup: %.2f times.\n",
               naive.decryption_time /
               fast.decryption_time);
    }

    printf("\nAlgorithm complexity:\n");
    printf("Naive:             O(exp)\n");
    printf("Square-and-Multiply: O(log exp)\n");
}

/* =====================================================
   MAIN FUNCTION
   ===================================================== */

int main(void)
{
    char plaintext[1024];

    printf("====================================\n");
    printf("       RSA PERFORMANCE TEST\n");
    printf("====================================\n");

    printf("\nRSA Parameters:\n");
    printf("P = %lu\n", RSA_P);
    printf("Q = %lu\n", RSA_Q);
    printf("N = %lu\n", RSA_N);
    printf("E = %lu\n", RSA_E);
    printf("D = %lu\n", RSA_D);

    /* Verify RSA key */

    if (!verify_rsa_key())
    {
        printf("\nProgram stopped: invalid RSA key.\n");
        return 1;
    }

    /* Edge case tests */

    if (!verify_edge_cases())
    {
        printf("\nProgram stopped: edge case test failed.\n");
        return 1;
    }

    /* Functional tests */

    if (!verify_round_trip_functions())
    {
        printf("\nProgram stopped: functional test failed.\n");
        return 1;
    }

    /* Read plaintext */

    printf("\nEnter plaintext (maximum 1023 characters):\n");

    if (fgets(plaintext, sizeof(plaintext), stdin) == NULL)
    {
        printf("Input error.\n");
        return 1;
    }

    plaintext[strcspn(plaintext, "\n")] = '\0';

    if (strlen(plaintext) == 0)
    {
        printf("Empty plaintext.\n");
        return 1;
    }

    printf("\nOriginal plaintext: %s\n", plaintext);

    /* Run naive implementation */

    BenchmarkResult naive =
        run_benchmark(plaintext, 0);

    display_result("NAIVE RSA IMPLEMENTATION", naive);

    /* Run optimized implementation */

    BenchmarkResult fast =
        run_benchmark(plaintext, 1);

    display_result("OPTIMIZED RSA IMPLEMENTATION", fast);

    /* Final comparison */

    display_comparison(naive, fast);

    printf("\n====================================\n");
    printf("          TEST COMPLETED\n");
    printf("====================================\n");

    return 0;
}
