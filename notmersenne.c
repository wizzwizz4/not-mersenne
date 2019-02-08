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
    static size_t allocated = 0;
    static size_t no_known = 1;
    static size_t i = 1;

    if (no_known >= allocated) {
        if (known_primes) {
            allocated <<= 1;
            known_primes = realloc(known_primes,
                                   allocated * sizeof(speedint));
        } else {
            allocated = 1024;
            known_primes = malloc(allocated * sizeof(speedint));
            known_primes[0] = 2;
            known_primes[1] = 3;
            return 2;
        }
        // Don't bother with NULL handling right now; we've got no recourse
        // but crashing anyway, so it'll just slow us down.
    }

    _primes_loop_start:
    i += 2;  // only try even numbers
    speedint j = isqrt(i);
    register speedint prime;
    for (speedint *k = known_primes; (prime = *k) < j; k += 1) {
        if (i % prime == 0) {
            goto _primes_loop_start;
        }
    }
    known_primes[no_known] = i;
    no_known += 1;
    return i;
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
