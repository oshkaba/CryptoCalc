// Benchmark helpers for controlled RSA and sieve timing tests.
// Inputs are bounded so long runs are measurable without freezing the UI.

#include "benchmark.h"
#include "math_utils.h"
#include "rsa.h"
#include <sstream>
#include <ctime>
#include <vector>
#include <iomanip>

using namespace std;

// Counts primes up to a limit using the Sieve of Eratosthenes.
static int countPrimesWithSieve(long long limit)
{
    vector<bool> isPrime;
    if(limit < 2)
    {
        return 0;
    }

    isPrime.assign((size_t)limit + 1, true);
    isPrime[0] = false;
    isPrime[1] = false;

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

    int count = 0;
    for(long long i = 2; i <= limit; i++)
    {
        if(isPrime[(size_t)i])
        {
            count++;
        }
    }

    return count;
}

// Measures repeated full RSA runs over fixed prime ranges.
BenchmarkResult benchmarkRSA(int runsPerRange)
{
    BenchmarkResult result;
    vector<long long> mins;
    vector<long long> maxes;
    volatile long long guard;

    result.testName = "RSA Benchmark";
    result.description = "Each row runs full RSA cycles many times: key generation, encryption, and decryption.";

    mins.push_back(50);
    maxes.push_back(100);

    mins.push_back(100);
    maxes.push_back(500);

    mins.push_back(500);
    maxes.push_back(1000);

    mins.push_back(1000);
    maxes.push_back(5000);

    mins.push_back(5000);
    maxes.push_back(10000);

    mins.push_back(10000);
    maxes.push_back(50000);

    guard = 0;

    for(int i = 0; i < (int)mins.size(); i++)
    {
        clock_t startTime;
        clock_t endTime;
        double elapsedMs;
        stringstream label;

        startTime = clock();

        for(int j = 0; j < runsPerRange; j++)
        {
            RSARun run = runRSADemo(42, mins[i], maxes[i]);
            guard += run.ciphertext;
            guard += run.decryptedMessage;
        }

        endTime = clock();
        elapsedMs = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;

        label << mins[i] << " to " << maxes[i];

        result.rowLabels.push_back(label.str());
        result.runs.push_back(runsPerRange);
        result.extraValues.push_back(42);
        result.totalTimesMs.push_back(elapsedMs);
        result.averageTimesMs.push_back(elapsedMs / runsPerRange);
    }

    if(guard == -1)
    {
        result.description += "";
    }

    return result;
}

// Measures sieve runtime over scaled limits up to the selected maximum.
BenchmarkResult benchmarkSieve(long long maxLimit)
{
    BenchmarkResult result;
    vector<long long> limits;
    vector<long long> presets;

    result.testName = "Sieve Benchmark";
    result.description = "Each row builds a complete Sieve of Eratosthenes table up to the limit.";

    presets.push_back(10000);
    presets.push_back(100000);
    presets.push_back(500000);
    presets.push_back(1000000);
    presets.push_back(3000000);
    presets.push_back(5000000);
    presets.push_back(10000000);

    for(int i = 0; i < (int)presets.size(); i++)
    {
        if(presets[i] <= maxLimit)
        {
            limits.push_back(presets[i]);
        }
    }

    if(limits.empty() || limits[(int)limits.size() - 1] != maxLimit)
    {
        limits.push_back(maxLimit);
    }

    for(int i = 0; i < (int)limits.size(); i++)
    {
        clock_t startTime;
        clock_t endTime;
        double elapsedMs;
        int primeCount;

        startTime = clock();
        primeCount = countPrimesWithSieve(limits[i]);
        endTime = clock();

        elapsedMs = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;

        result.rowLabels.push_back(to_string(limits[i]));
        result.runs.push_back(1);
        result.extraValues.push_back(primeCount);
        result.totalTimesMs.push_back(elapsedMs);
        result.averageTimesMs.push_back(elapsedMs);
    }

    return result;
}

// Formats benchmark timings with stable millisecond precision.
static string formatMs(double value)
{
    stringstream ss;

    ss.setf(ios::fixed);
    ss.precision(3);
    ss << value << " ms";

    return ss.str();
}

// Converts benchmark rows into a fixed-width table for monospaced output.
string benchmarkResultToString(const BenchmarkResult& result)
{
    stringstream ss;

    ss << result.testName << endl << endl;
    ss << result.description << endl << endl;

    if(result.testName == "RSA Benchmark")
    {
        ss << left << setw(18) << "Prime Range" << setw(10) << "Runs" << setw(16) << "Total Time" << setw(16) << "Average Time" << endl;
        ss << left << setw(18) << "-----------" << setw(10) << "----" << setw(16) << "----------" << setw(16) << "------------" << endl;

        for(int i = 0; i < (int)result.rowLabels.size(); i++)
        {
            ss << left << setw(18) << result.rowLabels[i] << setw(10) << result.runs[i] << setw(16) << formatMs(result.totalTimesMs[i]) << setw(16) << formatMs(result.averageTimesMs[i]) << endl;
        }
    }
    else
    {
        ss << left << setw(16) << "Limit" << setw(16) << "Primes Found" << setw(16) << "Time" << endl;
        ss << left << setw(16) << "-----" << setw(16) << "------------" << setw(16) << "----" << endl;

        for(int i = 0; i < (int)result.rowLabels.size(); i++)
        {
            ss << left << setw(16) << result.rowLabels[i] << setw(16) << result.extraValues[i] << setw(16) << formatMs(result.totalTimesMs[i]) << endl;
        }
    }

    return ss.str();
}
