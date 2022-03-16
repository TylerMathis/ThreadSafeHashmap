#include <iostream>
#include <vector>
#include "../src/Hashmap.h"

using std::cout;
using std::cin;
using std::vector;
using tshm::Hashmap;

const int LIM = 1'000'000;
const int NUM_LIM = 50'000;

int main() {
    vector<int> randoms(LIM);
    
    srand(time(NULL));
    for (auto &x : randoms)
        x = (rand() % NUM_LIM) - NUM_LIM / 2; 

    Hashmap<int, int> freq(504821);
    for (int x : randoms) {
        auto [_, val] = freq.get(x);
        freq.put(x, val + 1);
    }

    int search;
    cout << "Enter number to search for:\n";
    cin >> search;

    for (int x : randoms) {
        int needed = search - x;
        auto [found, val] = freq.get(needed);
        if (found) {
            cout << "We found " << val << " matches!\n";
            return 0;
        }
    }
    cout << "not found :(";

    return 0;
}