// Declares number-theory helpers shared across the cryptography tools.

#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <string>
#include <vector>

using namespace std;

// Computes the greatest common divisor.
long long gcd(long long a, long long b);
// Computes base^exponent mod mod.
long long modExp(long long base, long long exponent, long long mod);
// Computes gcd and Bezout coefficients.
long long extendedGcd(long long a, long long b, long long& x, long long& y);
// Computes a modular inverse when one exists.
long long modInverse(long long a, long long mod);
// Checks whether two values are coprime.
bool isCoprime(long long a, long long b);

// Checks primality with direct divisibility.
bool isPrimeBasic(long long n);
// Checks primality using the sieve helper.
bool isPrimeSieve(long long n);
// Returns a divisor found by sieve, or -1 for prime values.
long long smallestDivisorUsingSieve(long long n);

// Generates a random number in an inclusive range.
long long randomLongLong(long long minValue, long long maxValue);
// Generates a random odd number in a range.
long long generateRandomOdd(long long minValue, long long maxValue);
// Generates a random prime in a range.
long long generatePrime(long long minValue, long long maxValue);
// Counts odd primes in a range.
int countOddPrimesInRange(long long minValue, long long maxValue);
// Checks whether a range has enough primes for RSA.
bool hasAtLeastTwoOddPrimes(long long minValue, long long maxValue);

#endif
