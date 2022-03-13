test: tests/Test*.cpp
	for file in $^; do \
		g++ $${file} -O3 -std=c++17 -Wall -pthread -o test && ./test; \
		rm test; \
	done

test_hashmap: tests/TestHashmap.cpp
	g++ tests/TestHashmap.cpp -O3 -std=c++17 -Wall -pthread -o test && ./test;
	rm test;

test_lockable_linked_list: tests/TestLockableLinkedList.cpp
	g++ tests/TestLockableLinkedList.cpp -O3 -std=c++17 -Wall -pthread -o test && ./test;
	rm test;

test_lock_free_linked_list: tests/TestLockFreeLinkedList.cpp
	g++ tests/TestLockFreeLinkedList.cpp -O3 -std=c++17 -Wall -pthread -o test && ./test;
	rm test;

test_managed_hashmap: tests/TestManagedHashmap.cpp
	g++ tests/TestManagedHashmap.cpp -O3 -std=c++17 -Wall -pthread -o test && ./test;
	rm test;

test_markable_reference: tests/TestMarkableReference.cpp
	g++ tests/TestMarkableReference.cpp -O3 -std=c++17 -Wall -pthread -o test && ./test;
	rm test;

bench: benches/Bench*.cpp
	for file in $^; do \
		g++ $${file} -O0 -std=c++17 -Wall -pthread -o bench && ./bench; \
		rm bench; \
	done

bench_hashmap: benches/BenchHashmap.cpp
	g++ benches/BenchHashmap.cpp -O0 -std=c++17 -Wall -pthread -o bench && ./bench;
	rm bench;

bench_lockable_linked_list: benches/BenchLockableLinkedList.cpp
	g++ benches/BenchLockableLinkedList.cpp -O0 -std=c++17 -Wall -pthread -o bench && ./bench;
	rm bench;

bench_managed_hashmap: benches/BenchManagedHashmap.cpp
	g++ benches/BenchManagedHashmap.cpp -O0 -std=c++17 -Wall -pthread -o bench && ./bench;
	rm bench;

bench_stl_hashmap: benches/BenchStlHashmap.cpp
	g++ benches/BenchStlHashmap.cpp -O0 -std=c++17 -Wall -pthread -o bench && ./bench;
	rm bench;



clean:
	rm benches/bench;
	rm tests/test;
	rm a.out;
