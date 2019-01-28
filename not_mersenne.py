def not_mersenne(base) -> (int, int):
    """Find a repeating pattern in the units digit of the powers of two.
    If that pattern contains 1, then we can rule out an entire category of
    n values from the possibility of (2**n)-1 being prime.

    Example for base 5: 1, 2, 4, 3, 1, 2, 4, 3, 1, 2, 4, 3
                        therefore (2**(4n+0))-1 is never a Mersenne prime.

    Return a, b where am+b is the class of values of n being ruled out.
    """

    next_digit = {}
    digit = 1
    b = 0
    while digit not in next_digit:
        # next_digit[digit] assigned first.
        next_digit[digit] = digit = (digit * 2) % base
        b += 1
    loop = next_digit[digit]
    a = 1
    seen_one = digit == 1 or loop == 1
    while loop != digit:
        a += 1
        loop = next_digit[loop]
        seen_one = seen_one or loop == 1
    b -= a
    if seen_one:
        ## if b == 0: print(next_digit, file=__import__("sys").stderr)
        return a, b
    return None, None

def primes(known=[2,3,5,7,11]):
    yield from known
    i = known[-1]
    while True:
        i += 1
        for prime in known:
            if i % prime == 0:
                break
        else:
            yield i
            known.append(i)

if __name__ == "__main__":
    for base in primes():
        a, b = not_mersenne(base)
        if a is not None:
            print(a, b, sep="n+")
        else:
            print("no", base)
