#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <fstream>
#include "../src/Hashset.h"

namespace chrono = std::chrono;

using std::cout;
using std::vector;
using std::thread;
using std::ofstream;

using tshs::Hashset;

#define sz(x) (int)(x).size()

vector<int> CAPACITY_TESTS = {50'000, 500'000, 5'000'000};
vector<int> LIM_TESTS = {10'000, 100'000, 1'000'000};
vector<int> THREAD_TESTS = {1, 2, 4, 8, 16, 20};

int main() {
	cout << "\n\nBENCHING HASHSET\n\n";

	// Get random numbers for use later
	srand(time(NULL));
	vector<int> randoms(LIM_TESTS.back());
	for (int &x : randoms) x = rand();

	vector<vector<vector<long long>>> results(
			sz(CAPACITY_TESTS), vector<vector<long long>>(
				sz(LIM_TESTS), vector<long long>(
					sz(THREAD_TESTS), 0
					)));

	for (int i = 0; i < sz(CAPACITY_TESTS); i++) {
		for (int j = 0; j < sz(LIM_TESTS); j++) {
			for (int k = 0; k < sz(THREAD_TESTS); k++) {
				int CAPACITY = CAPACITY_TESTS[i];
				int LIM = LIM_TESTS[j];
				int THREADS = THREAD_TESTS[k];

				Hashset<int> set(CAPACITY);

				auto putJob = [&](int start, int end) {
					for (int i = start; i <= end; i++)
						set.insert(randoms[i]);
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

	ofstream res("analysis/data/hashset.csv");
	res << "capacity,limit,threads,runtime\n";
	for (int i = 0; i < sz(CAPACITY_TESTS); i++) {
		cout << "Tests for capacity " << CAPACITY_TESTS[i] << "\n";
		printf("%-15s|", "Limit\\Threads");
		for (int k = 0; k < sz(THREAD_TESTS); k++)
			printf(" %-7d|", THREAD_TESTS[k]);
		cout << "\n";
		for (int j = 0; j < sz(LIM_TESTS); j++) {
			printf("%-15d|", LIM_TESTS[j]);
			for (int k = 0; k < sz(THREAD_TESTS); k++) {
				printf(" %-5lldms|", results[i][j][k]);
				res <<
					CAPACITY_TESTS[i] << "," <<
					LIM_TESTS[j] << "," <<
					THREAD_TESTS[k] << "," <<
					results[i][j][k] << "\n";
			}
			cout << "\n";
		}
		cout << "\n";
	}

	res.close();
}
