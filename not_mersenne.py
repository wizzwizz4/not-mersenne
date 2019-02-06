def not_mersenne(base) -> int:
    """Find a repeating pattern in the units digit of the powers of two.
    If that pattern contains 1, then we can rule out an entire category of
    n values from the possibility of (2**n)-1 being prime.

    Example for base 5: 1, 2, 4, 3, 1, 2, 4, 3, 1, 2, 4, 3
                        therefore (2**(4n+0))-1 is never a Mersenne prime.

    Return a where am is the class of values of n being ruled out.
    """

    assert base % 2, "A multiple of two causes an infinite loop."
    assert 1 << base.bit_length() != base, "A power of two causes no solution."

    y = n = base.bit_length()
    digit = (1 << n) - base
    while digit != 1:
        d = y - digit.bit_length()
        digit <<= d
        n += d
        if digit < base:
            digit *= 2
            n += 1
        digit -= base
    return n

def isqrt(n):
    # Newton's method.
    x = n // 2
    y = 0
    while abs(x - y) > 1:
        y = x - (x*x - n)//(2*x)
        x = y - (y*y - n)//(2*y)
    return max(x, y)

def primes(known=[2,3,5,7,11]):
    yield from known
    i = known[-1]
    while True:
        i += 2
        j = isqrt(i)
        is_prime = True
        for prime in known:
            if i % prime == 0:
                is_prime = False
                break
            if prime > j:
                break
        if is_prime:
            yield i
            known.append(i)

def factorise(n):
    for p in primes.__defaults__[0]:
        if n == 1:
            return
        if n%p == 0:
            yield p
            while n%p == 0:
                n //= p

def certain(p):
    certain = set()
    uncertain = {}
    isqrts = {}
    last_power_of_two = 2
    last_exponent = 1
    for base in p:
        n = not_mersenne(base)
        try:
            factors = frozenset(f for f in factorise(n)
                                if 1/(f not in certain))
        except ZeroDivisionError:
            continue
        if n in factors:  # n is prime
            certain.add(n)
            yield n
        elif factors:
            uncertain[n] = factors
            isqrts[n] = isqrt(n)

        if base > last_power_of_two:
            last_exponent += 1
            last_power_of_two += 2
            for n in tuple(uncertain.keys()):
                try:
                    factors = frozenset(f for f in uncertain[n]
                                        if 1/(f not in certain))
                except ZeroDivisionError:
                    del uncertain[n]
                    del isqrts[n]
                if isqrts[n] < last_exponent:
                    del uncertain[n]
                    del isqrts[n]
                    certain.add(n)
                    yield n

if __name__ == "__main__":
    p = primes()
    next(p)  # skip 2
    print(end="(2^")
    try:
        for a in certain(p):
            print(a, end="n) - 1\n(2^")
    except KeyboardInterrupt:
        print(" …")
        raise
