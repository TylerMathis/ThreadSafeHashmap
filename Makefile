test: Test*.cpp
	for file in $^; do \
		g++ $${file} -std=c++17 -Wall -pthread && ./a.out; rm a.out; \
	done

