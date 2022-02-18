#include <iostream>
#include <chrono>
#include <vector>
#include "Hashmap.h"

namespace chrono = std::chrono;

using std::cout; using std::endl;
using std::vector;
using wfhm::ManagedHashmap;

#define sz(x) (int)(x).size()

vector<int> CAPACITY_TESTS = {8'675'309, 31'013};
vector<int> LIM_TESTS = {10'000, 100'000};
vector<int> THREAD_TESTS = {1, 2, 4};

int main() {
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

				ManagedHashmap<int, int> map(CAPACITY, THREADS);

				auto startTime = chrono::system_clock::now();
				for (int x = 0; x < LIM; x++)
					map.put(randoms[x], x);
				auto endTime = chrono::system_clock::now();

				auto totalTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
				results[i][j][k] = totalTime;
				cout << "LIM: " << LIM << "\nCAP: " << CAPACITY << "\nTHREADS: " << THREADS << "\nTIME: " << totalTime << "ms" << endl << endl;
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
				printf(" %-5ldms|", results[i][j][k]);
			cout << "\n";
		}
		cout << "\n";
	}
}
