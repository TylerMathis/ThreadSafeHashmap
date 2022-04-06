#include "../src/Hashmap.h"

int main() {

	// Create Hashmap from string->int backed by an AddOnlyLockFreeLinkedList
	tshm::Hashmap<std::string, int, ll::AddOnlyLockFreeLL> map(10007);

	// Associate "test" with 100
	map.put("test", 100);

	// Exists = true, value = 100
	auto [ exists, value ] = map.get("test");

	// Doesnt = false, defVal = 0
	auto [ doesnt, defVal ] = map.get("hehe");

	return 0;
}