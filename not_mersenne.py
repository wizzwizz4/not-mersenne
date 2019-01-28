def not_mersenne(base) -> int:
    """Find a repeating pattern in the units digit of the powers of two.
    If that pattern contains 1, then we can rule out an entire category of
    n values from the possibility of (2**n)-1 being prime.

    Example for base 5: 1, 2, 4, 3, 1, 2, 4, 3, 1, 2, 4, 3
                        therefore (2**(4n+0))-1 is never a Mersenne prime.

    Return a where am is the class of values of n being ruled out.
    """

    seen = set()
    digit = 1
    while digit not in seen:
        seen.add(digit)
        digit = (digit * 2) % base
    if digit == 1:
        return len(seen)

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
        a = not_mersenne(base)
        print(a, end="n\n")
