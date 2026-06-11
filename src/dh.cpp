// Diffie-Hellman helper functions for computing public values and shared keys.

#include "dh.h"
#include "math_utils.h"
#include <sstream>

using namespace std;

// Computes Diffie-Hellman public values and shared keys from validated inputs.
DHRun runDHExchange(long long p, long long g, long long a, long long b)
{
    DHRun run;

    run.p = p;
    run.g = g;
    run.a = a;
    run.b = b;

    run.A = modExp(run.g, run.a, run.p);
    run.B = modExp(run.g, run.b, run.p);

    run.sharedA = modExp(run.B, run.a, run.p);
    run.sharedB = modExp(run.A, run.b, run.p);

    return run;
}

// Formats a Diffie-Hellman run for saving or plain text output.
string dhRunToString(const DHRun& run)
{
    stringstream ss;

    ss << "Diffie-Hellman Run" << endl;
    ss << "Public values: p = " << run.p << ", g = " << run.g << endl;
    ss << "Private values: Alice a = " << run.a << ", Bob b = " << run.b << endl;
    ss << "1. Alice computes A = g^a mod p = " << run.A << endl;
    ss << "2. Bob computes B = g^b mod p = " << run.B << endl;
    ss << "3. Alice computes shared key = B^a mod p = " << run.sharedA << endl;
    ss << "4. Bob computes shared key = A^b mod p = " << run.sharedB << endl;

    if(run.sharedA == run.sharedB)
    {
        ss << "Result: shared keys match." << endl;
    }
    else
    {
        ss << "Result: shared keys do not match." << endl;
    }

    return ss.str();
}
