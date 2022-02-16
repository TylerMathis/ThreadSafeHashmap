#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include "LinkedList.h"

namespace chrono = std::chrono;

using std::cout;
using std::vector; using std::thread;
using ll::LinkedList;

vector<int> LIM_TESTS = {10, 100, 1'000, 10'000, 20'000};
vector<int> THREAD_TESTS = {2, 4, 6, 8};

int main() {
	for (int LIM : LIM_TESTS) {
		/*
		 * SEQUENTIAL BENCHING
		 */
		LinkedList<int> sequential;

		cout << "\nRunning benchmarks on LinkedList with " << LIM << " insertions...\n";

		auto startTime = chrono::system_clock::now();
		for (int x = 0; x < LIM; x++)
			sequential.add(x);
		auto endTime = chrono::system_clock::now();

		auto totalTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
		cout << "Sequential insertions: " << totalTime << "ms\n";

		for (int THREADS : THREAD_TESTS) {
			/*
			 * PARALLEL BENCHING
			 */
			LinkedList<int> parallel;

			auto addJob = [&parallel](int start, int lim, int threads) {
				for (int x = start; x < lim; x += threads)
					parallel.add(x);
			};

			vector<thread> jobs;
			startTime = chrono::system_clock::now();
			for (int thread = 0; thread < THREADS; thread++)
				jobs.emplace_back(addJob, thread, LIM, THREADS);
			for (thread &t : jobs)
				t.join();
			endTime = chrono::system_clock::now();

			totalTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
			cout << "Parallel (" << THREADS << " threads) insertions: " << totalTime << "ms\n";
		}
	}

    return 0;
}

