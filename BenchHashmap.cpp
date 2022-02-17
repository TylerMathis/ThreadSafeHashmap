#include <iostream>
#include <chrono>
#include <vector>
#include "Hashmap.h"

namespace chrono = std::chrono;

using std::cout; using std::endl;
using std::vector;
using wfhm::ManagedHashmap;

vector<int> CAPACITY_TESTS = {8'675'309, 818'309, 31'013};
vector<int> LIM_TESTS = {10'000, 100'000, 500'000};
vector<int> THREAD_TESTS = {1, 2, 4};

int main() {
	// Get random numbers for use later
	vector<int> randoms(LIM_TESTS.back());
	for (int &x : randoms) x = rand();

	for (int CAPACITY : CAPACITY_TESTS) {
		for (int LIM : LIM_TESTS) {
			for (int THREADS : THREAD_TESTS) {
				ManagedHashmap<int, int> map(CAPACITY, THREADS);

				auto startTime = chrono::system_clock::now();
				for (int x = 0; x < LIM; x++)
					map.put(randoms[x], x);
				auto endTime = chrono::system_clock::now();

				auto totalTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
				cout << "LIM: " << LIM << "\nCAP: " << CAPACITY << "\nTHREADS: " << THREADS << "\nTIME: " << totalTime << "ms" << endl << endl;
			}
		}
	}
}
