#include <assert.h>
#include <vector>
#include <thread>
#include <set>
#include <iostream>
#include "../src/LinkedList.h"

using std::vector; using std::thread;
using std::cout;
using ll::LockFreeLL;

int main() {
	cout << "\n\nTESTING LOCK FREE LINKED LIST...\n\n";
	/*
	 * SEQUENTIAL TESTING
	 */
	cout << "\nBEGINNING SEQUENTIAL CHECKS\n";
	cout << "---------------------------\n";
	cout << "Testing sequential add...\n";
	LockFreeLL<int> sequentialList;
	assert(sequentialList.size() == 0);
	for (int x = 0; x < 10; x++)
		sequentialList.add(x);
	assert(sequentialList.size() == 10);
	for (int x = 0; x < 10; x++) {
		int search = x;
		bool found = sequentialList.find(search);
		assert(found && search == x);
	}

	cout << "Testing sequential remove...\n";
	for (int x = 0; x < 10; x += 2)
		assert(sequentialList.remove(x));
	assert(sequentialList.size() == 5);
	for (int x = 0; x < 10; x++) {
		int search = x;
		bool found = sequentialList.find(search);
		if (x & 1) assert(found && search == x);
		else assert(!found);
	}

	cout << "Testing sequential remove full...\n";
	for (int x = 1; x < 10; x += 2)
		assert(sequentialList.remove(x));
	assert(sequentialList.size() == 0);

	/*
	 * THREADED TESTING
	 */
	LockFreeLL<int> threadedList;
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

	auto removeWorker = [&threadedList](int start, int lim, int inc) {
		for (int x = start; x < lim; x += inc)
			assert(threadedList.remove(x));
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
	auto head = threadedList.NOT_THREAD_SAFE_getHead();
	int found = 0;
	while (head.getRef() != nullptr)
		head = head.getRef()->next, found++;
	assert(found == LIM + 1);

	cout << "Checking threaded containment...\n";
	for (int thread = 0; thread < THREADS; thread++)
		jobs.emplace_back(checkWorker, thread, LIM, THREADS, true);
	for (thread &t : jobs)
		t.join();
	jobs.clear();

	cout << "Testing threaded removal...\n";
	for (int thread = 0; thread < THREADS; thread++)
		jobs.emplace_back(removeWorker, thread, LIM, THREADS);
	for (thread &t : jobs)
		t.join();
	jobs.clear();

	cout << "Testing containment after removal...\n";
	for (int thread = 0; thread < THREADS; thread++)
		jobs.emplace_back(checkWorker, thread, LIM, THREADS, false);
	for (thread &t : jobs)
		t.join();
	jobs.clear();

	cout << "\nSuccess :D\n";
	return 0;
}


