test: Test*.cpp
	for file in $^; do \
		g++ $${file} -std=c++17 -Wall && ./a.out; rm a.out; \
	done

