test: Test*.cpp
	g++ $^ -std=c++17 -Wall && ./a.out; rm a.out
