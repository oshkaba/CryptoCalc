// Declares Diffie-Hellman data structures and helper functions.

#ifndef DH_H
#define DH_H

#include <string>

using namespace std;

struct DHRun
{
    long long p;
    long long g;
    long long a;
    long long b;
    long long A;
    long long B;
    long long sharedA;
    long long sharedB;
};

// Runs a Diffie-Hellman key exchange.
DHRun runDHExchange(long long p, long long g, long long a, long long b);

// Formats Diffie-Hellman output.
string dhRunToString(const DHRun& run);

#endif