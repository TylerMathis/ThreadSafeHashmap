test: tests/Test*.cpp
	for file in $^; do \
		g++ $${file} -O3 -std=c++17 -Wall -pthread -o test && ./test; \
		rm test; \
	done

bench: benches/Bench*.cpp
	for file in $^; do \
		g++ $${file} -O0 -std=c++17 -Wall -pthread -o bench && ./bench; \
		rm bench; \
	done

clean:
	rm benches/bench;
	rm tests/test;
