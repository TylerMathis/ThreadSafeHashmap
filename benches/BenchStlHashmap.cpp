#include <iostream>
#include <chrono>
#include <vector>
#include <unordered_map>

namespace chrono = std::chrono;

using std::cout;
using std::vector;
using std::unordered_map;

#define sz(x) (int)(x).size()

vector<int> LIM_TESTS = {10'000, 100'000, 1'000'000};

int main() {
    cout << "\n\nBENCHING STL HASHMAP\n\n";

	// Get random numbers for use later
	srand(time(NULL));
	vector<int> randoms(LIM_TESTS.back());
	for (int &x : randoms) x = rand();

    for (int j = 0; j < sz(LIM_TESTS); j++) {
        int LIM = LIM_TESTS[j];

        unordered_map<int, int> hashmap;

        auto startTime = chrono::system_clock::now();
        for (int x = 0; x < LIM; x++)
            hashmap[randoms[x]] = x;
        auto endTime = chrono::system_clock::now();

        auto totalTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
        cout << "Lim: " << LIM << " " << totalTime << "ms\n";
    }
}
