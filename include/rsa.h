// Declares RSA data structures and helper functions.

#ifndef RSA_H
#define RSA_H

#include <string>
#include <vector>

using namespace std;

struct RSAKeys
{
    long long p;
    long long q;
    long long n;
    long long phi;
    long long e;
    long long d;
};

struct RSARun
{
    RSAKeys keys;
    long long message;
    long long ciphertext;
    long long decryptedMessage;
};

// Generates RSA keys from a prime range.
RSAKeys generateKeys(long long minPrime, long long maxPrime);

// Encrypts a numeric message.
long long encrypt(long long message, const RSAKeys& keys);

// Decrypts a numeric ciphertext.
long long decrypt(long long ciphertext, const RSAKeys& keys);

// Runs a complete RSA calculation.
RSARun runRSADemo(long long message, long long minPrime, long long maxPrime);

// Formats RSA key values.
string rsaKeysToString(const RSAKeys& keys);

// Formats a complete RSA run.
string rsaRunToString(const RSARun& run);

#endif