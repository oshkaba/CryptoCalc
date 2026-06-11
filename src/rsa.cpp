// RSA helper functions for small interactive key generation and encryption.
// This is for calculator-style experimentation, not production cryptography.

#include "rsa.h"
#include "math_utils.h"

#include <sstream>
#include <climits>

using namespace std;

// Builds a sentinel key object for failed RSA key generation.
static RSAKeys invalidKeys()
{
    RSAKeys keys;

    keys.p = -1;
    keys.q = -1;
    keys.n = -1;
    keys.phi = -1;
    keys.e = -1;
    keys.d = -1;

    return keys;
}

// Generates small RSA keys from a bounded prime range.
RSAKeys generateKeys(long long minPrime, long long maxPrime)
{
    RSAKeys keys;
    long long candidate;

    if(!hasAtLeastTwoOddPrimes(minPrime, maxPrime))
    {
        return invalidKeys();
    }

    keys.p = generatePrime(minPrime, maxPrime);
    keys.q = -1;

    candidate = keys.p + 1;
    while(candidate <= maxPrime)
    {
        if(isPrimeBasic(candidate))
        {
            keys.q = candidate;
            break;
        }

        candidate++;
    }

    if(keys.p == -1 || keys.q == -1 || keys.q == keys.p)
    {
        return invalidKeys();
    }

    if(keys.p != 0 && keys.q > LLONG_MAX / keys.p)
    {
        return invalidKeys();
    }

    keys.n = keys.p * keys.q;
    keys.phi = (keys.p - 1) * (keys.q - 1);

    keys.e = 3;

    while(keys.e < keys.phi)
    {
        if(isCoprime(keys.e, keys.phi))
        {
            break;
        }

        keys.e += 2;
    }

    keys.d = modInverse(keys.e, keys.phi);

    if(keys.d == -1)
    {
        return invalidKeys();
    }

    return keys;
}

// Encrypts a numeric message with the public RSA exponent.
long long encrypt(long long message, const RSAKeys& keys)
{
    return modExp(message, keys.e, keys.n);
}

// Decrypts a numeric ciphertext with the private RSA exponent.
long long decrypt(long long ciphertext, const RSAKeys& keys)
{
    return modExp(ciphertext, keys.d, keys.n);
}

// Runs key generation, encryption, and decryption as one RSA workflow.
RSARun runRSADemo(long long message, long long minPrime, long long maxPrime)
{
    RSARun run;

    run.keys = generateKeys(minPrime, maxPrime);
    run.message = message;

    if(run.keys.n == -1)
    {
        run.ciphertext = -1;
        run.decryptedMessage = -1;
        return run;
    }

    run.ciphertext = encrypt(message, run.keys);
    run.decryptedMessage = decrypt(run.ciphertext, run.keys);

    return run;
}

// Formats RSA key values for plain text output.
string rsaKeysToString(const RSAKeys& keys)
{
    stringstream ss;

    ss << "RSA Keys" << endl;
    ss << "p: " << keys.p << endl;
    ss << "q: " << keys.q << endl;
    ss << "n: " << keys.n << endl;
    ss << "phi: " << keys.phi << endl;
    ss << "e: " << keys.e << endl;
    ss << "d: " << keys.d << endl;

    return ss.str();
}

// Formats a complete RSA run for saving or display.
string rsaRunToString(const RSARun& run)
{
    stringstream ss;

    ss << "RSA" << endl;
    ss << "1. Choose two different primes:" << endl;
    ss << "   p = " << run.keys.p << ", q = " << run.keys.q << endl;
    ss << "2. Compute modulus:" << endl;
    ss << "   n = p * q = " << run.keys.n << endl;
    ss << "3. Compute Euler phi:" << endl;
    ss << "   phi = (p - 1)(q - 1) = " << run.keys.phi << endl;
    ss << "4. Choose public exponent:" << endl;
    ss << "   e = " << run.keys.e << endl;
    ss << "5. Compute private exponent:" << endl;
    ss << "   d = " << run.keys.d << endl;
    ss << "6. Encrypt:" << endl;
    ss << "   c = m^e mod n" << endl;
    ss << "   c = " << run.message << "^" << run.keys.e << " mod " << run.keys.n;
    ss << " = " << run.ciphertext << endl;
    ss << "7. Decrypt:" << endl;
    ss << "   m = c^d mod n" << endl;
    ss << "   m = " << run.ciphertext << "^" << run.keys.d << " mod " << run.keys.n;
    ss << " = " << run.decryptedMessage << endl;
    ss << "Public key: (e=" << run.keys.e << ", n=" << run.keys.n << ")" << endl;
    ss << "Private key: (d=" << run.keys.d << ", n=" << run.keys.n << ")" << endl;

    return ss.str();
}
