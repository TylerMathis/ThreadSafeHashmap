#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include "../src/Hashmap.h"

namespace chrono = std::chrono;

using std::cout;
using std::vector;
using wfhm::Hashmap;

#define sz(x) (int)(x).size()

vector<int> CAPACITY_TESTS = {8'675'309, 31'013, 10'007};
vector<int> LIM_TESTS = {10'000, 100'000, 1'000'000};
vector<int> THREAD_TESTS = {1, 2, 4, 8};

int main() {
    cout << "\n\nBENCHING HASHMAP\n\n";

	// Get random numbers for use later
	srand(time(NULL));
	vector<int> randoms(LIM_TESTS.back());
	for (int &x : randoms) x = rand();

	vector<vector<vector<int64_t>>> results(
		sz(CAPACITY_TESTS), vector<vector<int64_t>>(
			sz(LIM_TESTS), vector<int64_t>(
				sz(THREAD_TESTS), 0
	)));

	for (int i = 0; i < sz(CAPACITY_TESTS); i++) {
		for (int j = 0; j < sz(LIM_TESTS); j++) {
			for (int k = 0; k < sz(THREAD_TESTS); k++) {
				int CAPACITY = CAPACITY_TESTS[i];
				int LIM = LIM_TESTS[j];
				int THREADS = THREAD_TESTS[k];

				Hashmap<int, int> map(CAPACITY);

                auto putJob = [&](int start, int end) {
                    for (int i = start; i <= end; i++)
                        map.put(randoms[i], i);
                };


                int gap = LIM / THREADS;
                vector<thread> threads;

				auto startTime = chrono::system_clock::now();

                for (int thread = 0; thread < THREADS; thread++)
                    threads.emplace_back(putJob, thread * gap, thread * gap + gap);
                for (thread &t : threads)
                    t.join();

				auto endTime = chrono::system_clock::now();

				auto totalTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
				results[i][j][k] = totalTime;
			}
		}
	}

	cout << "Results summary:\n";
	cout << "----------------\n";

	for (int i = 0; i < sz(CAPACITY_TESTS); i++) {
		cout << "Tests for capacity " << CAPACITY_TESTS[i] << "\n";
		printf("%-15s|", "Limit\\Threads");
			for (int k = 0; k < sz(THREAD_TESTS); k++)
				printf(" %-7d|", THREAD_TESTS[k]);
		cout << "\n";
		for (int j = 0; j < sz(LIM_TESTS); j++) {
			printf("%-15d|", LIM_TESTS[j]);
			for (int k = 0; k < sz(THREAD_TESTS); k++)
				printf(" %-5lldms|", results[i][j][k]);
			cout << "\n";
		}
		cout << "\n";
	}
}
