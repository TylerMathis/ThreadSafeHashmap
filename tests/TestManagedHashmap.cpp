#include <iostream>
#include <string>
#include <assert.h>
#include <functional>
#include "../src/Hashmap.h"

using std::cout;
using std::string;

using tshm::ManagedHashmap;

const int LARGE_PRIME = 8675309;

int main() {
	cout << "\n\nMANAGED HASHMAP TESTING...\n\n";

	cout << "Testing single value...\n";
	ManagedHashmap<string, int> hashmap(LARGE_PRIME);
	hashmap.put("test", 5);
	int *value = hashmap.getSequential("test");
	assert(value != nullptr && *value == 5);

	cout << "Testing threaded put...\n";
	for (int i = 0; i < 26; i++) {
		string key(1, 'a' + i);
		hashmap.put(key, i);
	}

	cout << "Testing sequential get...\n";
	for (int i = 0; i < 26; i++) {
		string key(1, 'a' + i);
		value = hashmap.getSequential(key);
		assert(value != nullptr && *value == i);
	}

	cout << "Testing threaded overwrites...\n";
	for (int i = 0; i < 26; i++) {
		string key(1, 'a' + i);
		hashmap.put(key, 2*i);
	}
	for (int i = 0; i < 26; i++) {
		string key(1, 'a' + i);
		value = hashmap.getSequential(key);
		assert(value != nullptr && *value == 2*i);
	}

	cout << "\nSuccess :D\n";

    return 0;
}

