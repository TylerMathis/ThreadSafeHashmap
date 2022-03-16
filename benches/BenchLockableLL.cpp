#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <fstream>
#include "../src/LinkedList.h"

namespace chrono = std::chrono;

using std::cout;
using std::vector;
using std::thread;
using std::ofstream;

using ll::LockableLL;

vector<int> LIM_TESTS = {10, 100, 1'000, 10'000, 20'000};
vector<int> THREAD_TESTS = {2, 4};

int main() {
    cout << "\n\nBENCHING LOCKABLE LINKED LIST\n\n";

    ofstream res("analysis/data/lockable_ll.csv");
    res << "limit,threads,runtime\n";
	for (int LIM : LIM_TESTS) {
		/*
		 * SEQUENTIAL BENCHING
		 */
		LockableLL<int> sequential;

		cout << LIM << " insertions...\n";

		auto startTime = chrono::system_clock::now();
		for (int x = 0; x < LIM; x++)
			sequential.add(x);
		auto endTime = chrono::system_clock::now();

		auto totalTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
		cout << "Sequential insertions: " << totalTime << "ms\n";
        res << LIM << "," << 1 << "," << totalTime << "\n";

		for (int THREADS : THREAD_TESTS) {
			/*
			 * PARALLEL BENCHING
			 */
			LockableLL<int> parallel;

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
            res << LIM << "," << THREADS << "," << totalTime << "\n";
		}
	}

    return 0;
}

