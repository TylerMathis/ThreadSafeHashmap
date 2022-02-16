test: Test*.cpp
	for file in $^; do \
		g++ $${file} -std=c++17 -Wall -pthread && ./a.out; \
	done

bench: Bench*.cpp
	for file in $^; do \
		g++ $${file} -std=c++17 -Wall -pthread && ./a.out; \
	done

clean:
	rm a.out
