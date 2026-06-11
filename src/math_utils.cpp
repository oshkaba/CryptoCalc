// Number-theory helpers used by RSA, Diffie-Hellman, primality checks, and Math Tools.
// These routines are bounded for interactive demo-sized calculations.

#include "math_utils.h"
#include <cstdlib>
#include <vector>

using namespace std;

// Computes the greatest common divisor with the Euclidean algorithm.
long long gcd(long long a, long long b)
{
    if(a < 0)
    {
        a = -a;
    }

    if(b < 0)
    {
        b = -b;
    }

    while(b != 0)
    {
        long long temp = b;
        b = a % b;
        a = temp;
    }

    return a;
}

// Multiplies under a modulus while reducing overflow risk for demo-sized values.
static long long modMultiply(long long a, long long b, long long mod)
{
    __int128 result;

    result = (__int128)a * (__int128)b;
    result = result % mod;

    return (long long)result;
}

// Computes base^exponent mod mod using repeated squaring.
long long modExp(long long base, long long exponent, long long mod)
{
    if(mod <= 0)
    {
        return -1;
    }

    if(mod == 1)
    {
        return 0;
    }

    long long result = 1;
    base = base % mod;

    while(exponent > 0)
    {
        if(exponent % 2 == 1)
        {
            result = modMultiply(result, base, mod);
        }

        base = modMultiply(base, base, mod);
        exponent = exponent / 2;
    }

    return result;
}

// Computes gcd and Bezout coefficients for modular inverse calculation.
long long extendedGcd(long long a, long long b, long long& x, long long& y)
{
    if(b == 0)
    {
        x = 1;
        y = 0;
        return a;
    }

    long long x1;
    long long y1;
    long long gcdValue = extendedGcd(b, a % b, x1, y1);

    x = y1;
    y = x1 - (a / b) * y1;

    return gcdValue;
}

// Returns the modular inverse of a modulo mod, or -1 when none exists.
long long modInverse(long long a, long long mod)
{
    long long x;
    long long y;
    long long gcdValue = extendedGcd(a, mod, x, y);

    if(gcdValue != 1)
    {
        return -1;
    }

    long long inverse = x % mod;

    if(inverse < 0)
    {
        inverse += mod;
    }

    return inverse;
}

// Checks whether two numbers have gcd 1.
bool isCoprime(long long a, long long b)
{
    return gcd(a, b) == 1;
}


// Performs a direct primality check for small helper cases.
bool isPrimeBasic(long long n)
{
    if(n < 2)
    {
        return false;
    }

    if(n == 2)
    {
        return true;
    }

    if(n % 2 == 0)
    {
        return false;
    }

    for(long long i = 3; i * i <= n; i += 2)
    {
        if(n % i == 0)
        {
            return false;
        }
    }

    return true;
}

// Computes floor(sqrt(n)) without floating-point rounding concerns.
static long long integerSquareRoot(long long n)
{
    long long low;
    long long high;
    long long answer;
    long long mid;

    if(n <= 0)
    {
        return 0;
    }

    low = 1;
    high = n;
    answer = 1;

    while(low <= high)
    {
        mid = low + (high - low) / 2;

        if(mid <= n / mid)
        {
            answer = mid;
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    return answer;
}

// Finds a divisor of n by sieving possible factors up to sqrt(n).
long long smallestDivisorUsingSieve(long long n)
{
    long long limit;
    vector<bool> isPrime;

    if(n < 2)
    {
        return 1;
    }

    if(n % 2 == 0)
    {
        if(n == 2)
        {
            return -1;
        }

        return 2;
    }

    limit = integerSquareRoot(n);
    isPrime.assign((size_t)limit + 1, true);

    if(limit >= 0)
    {
        isPrime[0] = false;
    }

    if(limit >= 1)
    {
        isPrime[1] = false;
    }

    for(long long i = 2; i <= limit / i; i++)
    {
        if(isPrime[(size_t)i])
        {
            for(long long j = i * i; j <= limit; j += i)
            {
                isPrime[(size_t)j] = false;
            }
        }
    }

    for(long long i = 3; i <= limit; i += 2)
    {
        if(isPrime[(size_t)i] && n % i == 0)
        {
            return i;
        }
    }

    return -1;
}

// Checks primality by asking whether the sieve can find a divisor.
bool isPrimeSieve(long long n)
{
    return smallestDivisorUsingSieve(n) == -1;
}

// Generates a random value in the inclusive range used by demo key generation.
long long randomLongLong(long long minValue, long long maxValue)
{
    if(minValue > maxValue)
    {
        long long temp = minValue;
        minValue = maxValue;
        maxValue = temp;
    }

    long long range = maxValue - minValue + 1;
    long long value = rand() % range;

    return minValue + value;
}

// Generates an odd candidate so RSA prime search avoids even numbers.
long long generateRandomOdd(long long minValue, long long maxValue)
{
    if(minValue > maxValue)
    {
        long long temp = minValue;
        minValue = maxValue;
        maxValue = temp;
    }

    if(minValue % 2 == 0)
    {
        minValue++;
    }

    if(minValue > maxValue)
    {
        return -1;
    }

    while(true)
    {
        long long value = randomLongLong(minValue, maxValue);

        if(value % 2 == 1)
        {
            return value;
        }
    }
}

// Counts distinct odd primes in a range before RSA tries to choose p and q.
int countOddPrimesInRange(long long minValue, long long maxValue)
{
    int count;
    long long value;

    if(minValue > maxValue)
    {
        return 0;
    }

    if(minValue < 3)
    {
        minValue = 3;
    }

    if(minValue % 2 == 0)
    {
        minValue++;
    }

    count = 0;
    value = minValue;

    while(value <= maxValue)
    {
        if(isPrimeSieve(value))
        {
            count++;

            if(count >= 2)
            {
                return count;
            }
        }

        value += 2;
    }

    return count;
}

// Confirms an RSA range can supply two different odd primes.
bool hasAtLeastTwoOddPrimes(long long minValue, long long maxValue)
{
    return countOddPrimesInRange(minValue, maxValue) >= 2;
}

// Attempts to choose a random odd prime from the requested range.
long long generatePrime(long long minValue, long long maxValue)
{
    long long value;

    if(minValue > maxValue)
    {
        return -1;
    }

    if(maxValue < 2)
    {
        return -1;
    }

    if(minValue < 2)
    {
        minValue = 2;
    }

    value = minValue;
    while(value <= maxValue)
    {
        if(isPrimeSieve(value))
        {
            return value;
        }

        value++;
    }

    return -1;
}
