test: Test*.cpp
	for file in $^; do \
		g++ $${file} -O3 -std=c++17 -Wall -pthread -O2 && ./a.out; \
	done

bench: Bench*.cpp
	for file in $^; do \
		g++ $${file} -O3 -std=c++17 -Wall -pthread && ./a.out; \
	done

clean:
	rm a.out
