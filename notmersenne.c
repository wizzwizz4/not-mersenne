#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>

typedef uint_fast64_t speedint;
#define PRIspeedint PRIuFAST64
#define SCNspeedint SCNuFAST64

speedint const high_bit = (sizeof(speedint) * CHAR_BIT) - 1;

speedint not_mersenne(speedint base) {
    assert(base & 1);  // A multiple of two causes an infinite loop.

    speedint y = __builtin_clzll(base);
    // ( same as high_bit - y because (1<<n)-1 ) + 1
    speedint n = (high_bit ^ y) + 1;
    speedint digit = (1 << n) - base;
    while (digit != 1) {
        // // (63 - y) - x == 63 - (y + x) = 63 ^ (y + x)
        // (63 - y) - (63 - x) == -y - -x == x - y
        speedint d = __builtin_clzll(digit) - y;
        digit <<= d;
        n += d;
        if (digit < base) {
            digit *= 2;
            n += 1;
        }
        digit -= base;
    }
    return n;
}

speedint isqrt(speedint n) {
    // optimise me for I am slow
    speedint x = n;
    speedint y = 0;
    while ((x & -2) != (y & -2)) {
        y = x - (x*x - n) / (2*x);
        x = y - (y*y - n) / (2*y);
    }
    return (x > y) ? x : y;
}

speedint *known_primes = NULL;

speedint primes() {
    static size_t calculated = 2;
    static size_t output_index = -2;

    if (++output_index < calculated) {
        return known_primes[output_index];
    }
    output_index -= 1;

    if (!known_primes) {
        known_primes = malloc(sizeof(speedint));
        output_index = 0;
        known_primes[1] = 3;
        return (known_primes[0] = 2);
    }

    // todo: optimise to not bother with 2 – is that possible?

    register speedint lower = known_primes[output_index];
    register speedint upper = lower * lower;
    lower += 2;
    bool sieve_arr[upper - lower];
    bool *sieve = sieve_arr - lower;
    // zero
    for (size_t i = lower; i < upper; i += 2) {
        sieve[i] = true;
    }
    // sieve
    for (size_t prime_i = 1; prime_i <= output_index; prime_i += 1) {
        speedint prime = known_primes[prime_i];
        for (speedint i = lower + ((lower % prime) ? prime - (lower % prime)
                                                   : 0);
             i < upper;
             i += prime) {
           sieve[i] = false;
        }
    }
    // allocate
    for (size_t i = lower; i < upper; i += 2) {
        if (sieve[i]) {
            calculated += 1;
        }
    }
    known_primes = realloc(known_primes,
                           calculated * sizeof(speedint));
    // save
    speedint *destination = &known_primes[output_index];
    for (size_t i = lower; i < upper; i += 2) {
        if (sieve[i]) {
            *(++destination) = i;
        }
    }
    return known_primes[++output_index];
}

typedef struct {
    speedint prime;
    speedint exponent;
} factor;

factor *factorise(speedint n, size_t *length_out) {
    assert(n > 1);
    size_t length = 0;
    size_t allocated = 32;
    speedint *primes = known_primes;
    factor *factors;
    factors = NULL;
    while (1) {
        allocated <<= 1;
        factors = realloc(factors, allocated * sizeof(factor));
        while (length < allocated) {
            register speedint prime = *primes;
            if (n % prime == 0) {
                register speedint exponent = 0;
                do {
                    n /= prime;
                    exponent += 1;
                } while (n % prime == 0);
                factors[length].prime = prime,
                factors[length].exponent = exponent;
                length += 1;
            }
            primes += 1;
            if (n == 1) {
                factors = realloc(factors, length * sizeof(factor));
                *length_out = length;
                return factors;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        speedint a;
        sscanf(argv[1], "%" SCNspeedint, &a);
        { speedint b = a; while (--b) primes(); }
        size_t factorc;
        factor *factorv = factorise(a, &factorc);
        printf("Factors of %" PRIspeedint "\n", a);
        for (size_t i = 0; i < factorc; i += 1) {
            printf("  * %" PRIspeedint " ^ %" PRIspeedint "\n",
                   factorv[i].prime, factorv[i].exponent);
        }
        exit(0);
    }

    primes();  // initialise, ignore 2
    while (1) {
        printf("%" PRIspeedint "\n", not_mersenne(primes()));
    }
}
