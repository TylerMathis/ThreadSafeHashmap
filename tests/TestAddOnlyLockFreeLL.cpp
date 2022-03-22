#include <assert.h>
#include <vector>
#include <thread>
#include <set>
#include <iostream>
#include "../src/LinkedList.h"

using std::vector;
using std::thread;
using std::cout;

using ll::AddOnlyLockFreeLL;

int main() {
	cout << "\n\nADD ONLY LOCK FREE LINKED LIST TESTING...\n\n";
	/*
	 * SEQUENTIAL TESTING
	 */
	cout << "\nBEGINNING SEQUENTIAL CHECKS\n";
	cout << "---------------------------\n";
	cout << "Testing sequential add...\n";
	AddOnlyLockFreeLL<int> sequentialList;
	assert(sequentialList.size() == 0);
	for (int x = 0; x < 10; x++)
		sequentialList.add(x);
	assert(sequentialList.size() == 10);
	for (int x = 0; x < 10; x++) {
		int search = x;
		assert(sequentialList.find(search) && search == x);
	}

	/*
	 * THREADED TESTING
	 */
	AddOnlyLockFreeLL<int> threadedList;
	vector<thread> jobs;

	auto addWorker = [&threadedList](int start, int lim, int inc) {
		for (int x = start; x < lim; x += inc)
			threadedList.add(x);
	};

	auto checkWorker = [&threadedList](int start, int lim, int inc, bool exists) {
		for (int x = start; x < lim; x += inc) {
			int search = x;
			bool found = threadedList.find(search);
			if (exists) assert(found && search == x);
			else assert(!found);
		}
	};

	const int THREADS = 4, LIM = 1'000;
	cout << "\nBEGINNING THREADED CHECKS\n";
	cout << "Threads: " << THREADS << "\n";
	cout << "Elements: " << LIM << "\n";
	cout << "-------------------------\n";
	cout << "Testing threaded add...\n";
	for (int thread = 0; thread < THREADS; thread++)
		jobs.emplace_back(addWorker, thread, LIM, THREADS);
	for (thread &t : jobs)
		t.join();
	jobs.clear();

	cout << "Checking size and connectivity...\n";
	assert(threadedList.size() == LIM);
	auto *head = threadedList.NOT_THREAD_SAFE_getHead();
	int found = 0;
	while (head != nullptr)
		head = head->next, found++;
	assert(found == LIM + 1);

	cout << "Checking threaded containment...\n";
	for (int thread = 0; thread < THREADS; thread++)
		jobs.emplace_back(checkWorker, thread, LIM, THREADS, true);
	for (thread &t : jobs)
		t.join();
	jobs.clear();

	cout << "\nSuccess :D\n";
	return 0;
}

