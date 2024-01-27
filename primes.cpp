#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <cmath>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <numeric>

using namespace std;

const int NUM_THREADS = 8;
const int MAX_NUMBER = 100000000;

vector<int> primes;
mutex primesMutex;
int currentNumberToCheck = 1;
mutex numberMutex;

bool isPrime(int num) {
    int sqrtNum = sqrt(num);
    for (int i = 2; i <= sqrtNum; ++i) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

void findPrimes() {
    while (true) {
        unique_lock<mutex> lock(numberMutex);
        int numToCheck = currentNumberToCheck;
        if (numToCheck > MAX_NUMBER) {
            return;  // All numbers checked
        }
        currentNumberToCheck++;
        lock.unlock();

        if (isPrime(numToCheck)) {
            lock_guard<mutex> lock(primesMutex);
            primes.push_back(numToCheck);
        }
    }
}

int main() {
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    vector<thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(findPrimes);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    chrono::duration<double> elapsed_seconds = end - begin;

    // Writing to primes.txt
    ofstream outfile("primes.txt");
    if (outfile.is_open()) {
        
        outfile << elapsed_seconds.count() << ", " << primes.size() << ", ";

        long long sum = 0;
        for (int prime : primes) {
            sum += prime;
        }
        outfile << sum << ", ";

        sort(primes.begin(), primes.end());
        for (int i = primes.size() - 10; i < primes.size(); ++i) {
            outfile << primes[i] << ", ";
        }

        outfile.close();
    } else {
        cerr << "Unable to open primes.txt for writing." << endl;
        return 1;
    }

    return 0;
}
