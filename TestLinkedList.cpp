#include <assert.h>
#include "LinkedList.h"

using ll::LinkedList;

int main() {
	LinkedList<int> linkedList;

	assert(linkedList.size() == 0);
	for (int i = 0; i < 10; i++)
		linkedList.add(i);
	assert(linkedList.size() == 10);
	for (int i = 0; i < 10; i++)
		assert(linkedList.get(i) != nullptr);

	for (int i = 0; i < 10; i += 2)
		linkedList.remove(i);
	assert(linkedList.size() == 5);
	for (int i = 0; i < 10; i++) {
		if (i & 1) assert(linkedList.get(i) != nullptr);
		else assert(linkedList.get(i) == nullptr);
	}

	return 0;
}

