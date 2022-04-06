test: tests/Test*.cpp
	for file in $^; do \
		g++ $${file} -O3 -std=c++17 -Wall -pthread -o test && ./test; \
		rm test; \
	done

test_hashmap: tests/TestHashmap.cpp
	g++ tests/TestHashmap.cpp -O3 -std=c++17 -Wall -pthread -o test && ./test;
	rm test;

test_hashset: tests/TestHashset.cpp
	g++ tests/TestHashset.cpp -O3 -std=c++17 -Wall -pthread -o test && ./test;
	rm test;

test_lockable_ll: tests/TestLockableLL.cpp
	g++ tests/TestLockableLL.cpp -O3 -std=c++17 -Wall -pthread -o test && ./test;
	rm test;

test_lock_free_ll: tests/TestLockFreeLL.cpp
	g++ tests/TestLockFreeLL.cpp -O3 -std=c++17 -Wall -pthread -o test && ./test;
	rm test;

test_add_only_lock_free_ll: tests/TestAddOnlyLockFreeLL.cpp
	g++ tests/TestAddOnlyLockFreeLL.cpp -O3 -std=c++17 -Wall -pthread -o test && ./test;
	rm test;

test_managed_hashmap: tests/TestManagedHashmap.cpp
	g++ tests/TestManagedHashmap.cpp -O3 -std=c++17 -Wall -pthread -o test && ./test;
	rm test;



bench: benches/Bench*.cpp
	for file in $^; do \
		g++ $${file} -O0 -std=c++17 -Wall -pthread -o bench && ./bench; \
		rm bench; \
	done

bench_hashmap: benches/BenchHashmap.cpp
	g++ benches/BenchHashmap.cpp -O0 -std=c++17 -Wall -pthread -o bench && ./bench;
	rm bench;

bench_add_only_hashmap: benches/BenchAddOnlyHashmap.cpp
	g++ benches/BenchAddOnlyHashmap.cpp -O0 -std=c++17 -Wall -pthread -o bench && ./bench;
	rm bench;

bench_hashset: benches/BenchHashset.cpp
	g++ benches/BenchHashset.cpp -O0 -std=c++17 -Wall -pthread -o bench && ./bench;
	rm bench;

bench_lockable_ll: benches/BenchLockableLL.cpp
	g++ benches/BenchLockableLL.cpp -O0 -std=c++17 -Wall -pthread -o bench && ./bench;
	rm bench;

bench_add_only_lock_free_ll: benches/BenchAddOnlyLockFreeLL.cpp
	g++ benches/BenchAddOnlyLockFreeLL.cpp -O0 -std=c++17 -Wall -pthread -o bench && ./bench;
	rm bench;

bench_managed_hashmap: benches/BenchManagedHashmap.cpp
	g++ benches/BenchManagedHashmap.cpp -O0 -std=c++17 -Wall -pthread -o bench && ./bench;
	rm bench;

bench_stl_hashmap: benches/BenchStlHashmap.cpp
	g++ benches/BenchStlHashmap.cpp -O0 -std=c++17 -Wall -pthread -o bench && ./bench;
	rm bench;



clean:
	rm a.out;
	rm benches/bench;
	rm tests/test;
