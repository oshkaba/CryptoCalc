// Declares benchmark result data and formatting helpers.

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <vector>

using namespace std;

struct BenchmarkResult
{
    string testName;
    string description;
    vector<string> rowLabels;
    vector<int> runs;
    vector<long long> extraValues;
    vector<double> totalTimesMs;
    vector<double> averageTimesMs;
};

// Measures repeated RSA workflow runtime.
BenchmarkResult benchmarkRSA(int runsPerRange);
// Measures sieve runtime for scaled limits.
BenchmarkResult benchmarkSieve(long long maxLimit);

// Formats benchmark rows as a table.
string benchmarkResultToString(const BenchmarkResult& result);

#endif
