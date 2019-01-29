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

    digit = 2
    n = 1
    while digit != 1:
        digit = (digit * 2) % base
        n += 1
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

if __name__ == "__main__":
    p = primes()
    next(p)  # skip 2
    for base in p:
        a = not_mersenne(base)
        print(a, end="n\n")
