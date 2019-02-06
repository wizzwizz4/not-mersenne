#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>

typedef uint_fast64_t speedint;
#define PRIspeedint PRIuFAST64

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

speedint primes() {
    static size_t allocated = 0;
    static speedint *known = NULL;
    static size_t no_known = 0;
    static size_t i = 1;

    if (no_known >= allocated) {
        if (known) {
            allocated <<= 1;
            known = realloc(known, allocated * sizeof(speedint));
        } else {
            allocated = 1024;
            known = malloc(allocated * sizeof(speedint));
            known[0] = 2;
            return 2;
        }
        // Don't bother with NULL handling right now; we've got no recourse
        // but crashing anyway, so it'll just slow us down.
    }

    _primes_loop_start:
    i += 2;  // only try even numbers
    speedint j = isqrt(i);
    register speedint prime;
    for (speedint *k = known; (prime = *k) < j; k += 1) {
        if (i % prime == 0) {
            goto _primes_loop_start;
        }
    }
    known[no_known] = i;
    no_known += 1;
    return i;
}

int main(int argc, char *argv[]) {
    primes();  // initialise, ignore 2
    while (1) {
        printf("%" PRIspeedint "\n", not_mersenne(primes()));
    }
}
