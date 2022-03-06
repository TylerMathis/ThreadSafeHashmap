#include <assert.h>
#include <vector>
#include <thread>
#include <set>
#include <iostream>
#include "../src/LinkedList.h"

using std::vector; using std::thread;
using std::cout;
using ll::LockableLinkedList;

int main() {
	cout << "\n\nLINKED LIST TESTING...\n\n";
	/*
	 * SEQUENTIAL TESTING
	 */
	cout << "\nBEGINNING SEQUENTIAL CHECKS\n";
	cout << "---------------------------\n";
	cout << "Testing sequential add...\n";
	LockableLinkedList<int> sequentialList;
	assert(sequentialList.size() == 0);
	for (int x = 0; x < 10; x++)
		sequentialList.add(x);
	assert(sequentialList.size() == 10);
	for (int x = 0; x < 10; x++) {
		auto found = sequentialList.get(x);
		assert(found != nullptr && *found == x);
	}

	cout << "Testing sequential remove...\n";
	for (int x = 0; x < 10; x += 2)
		assert(sequentialList.remove(x));
	assert(sequentialList.size() == 5);
	for (int x = 0; x < 10; x++) {
		auto found = sequentialList.get(x);
		if (x & 1) assert(found != nullptr && *found == x);
		else assert(found == nullptr);
	}

	cout << "Testing sequential remove full...\n";
	for (int x = 1; x < 10; x += 2)
		assert(sequentialList.remove(x));
	assert(sequentialList.size() == 0);

	/*
	 * THREADED TESTING
	 */
	LockableLinkedList<int> threadedList;
	vector<thread> jobs;

	auto addWorker = [&threadedList](int start, int lim, int inc) {
		for (int x = start; x < lim; x += inc)
			threadedList.add(x);
	};

	auto checkWorker = [&threadedList](int start, int lim, int inc, bool exists) {
		for (int x = start; x < lim; x += inc) {
			auto found = threadedList.get(x);
			if (exists) assert(found != nullptr && *found == x);
			else assert(found == nullptr);
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
	auto *head = threadedList.DANGEROUS_getHead();
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

