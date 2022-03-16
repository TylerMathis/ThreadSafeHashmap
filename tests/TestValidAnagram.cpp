#include <iostream>
#include <string>
#include <chrono>
#include "../src/Hashmap.h"

using std::cout;
using std::cin;
using std::vector;
using tshm::Hashmap;
using std::string;
namespace chrono = std::chrono;

void testAnagramNormal(string str1, string str2) {
    int len = str1.length();
    Hashmap<char, int> anagrams(504821);

    for (int i = 0; i < len; i++) {
        auto res = anagrams.get(str1[i]);
        anagrams.put(str1[i], res.second + 1);

        res = anagrams.get(str2[i]);
        anagrams.put(str2[i], res.second - 1);
    }

    for (int i = 0; i < len; i++) {
        auto res = anagrams.get(str1[i]);
        if (res.second != 0) {
            cout << "Booo. Not an anagram. :(\n";
            return;
        }
    }

    cout << "Yay! Your strings are valid anagrams! :)\n";
}

int testAnagramParallel(string str1, string str2) {
    int len = str1.length();
    Hashmap<char, int> anagrams(len);


    // auto putJob = [&](int start, int end) {
    //     for (int i = start; i <= end; i++)
    //         hashmap.put(rands[i], i);
    // };

//     cout << "Testing threaded put for strings...\n";
//     vector<thread> threads;
//     for (int i = 0; i < len; i++)
//         threads.emplace_back(putJob, i*10, 10);
//     for (thread &t : threads)
//         t.join();
//     threads.clear();

//     cout << "Testing threaded get...\n";
//     for (int i = 0; i < 10; i++)
//         threads.emplace_back(getJob, i*10, 10);
//     for (thread &t : threads)
//         t.join();
//     threads.clear();

	// cout << "\nSuccess :D\n";
    return 0;
}

int main() {
    cout << "Test Valid Anagrams!\n";

    string str1, str2;
    cout << "Enter first string.\n";
    cin >> str1;
    cout << "Enter second string.\n";
    cin >> str2;

    if (str1.length() != str2.length()) {
        cout << "Booo. Not an anagram. :(\n";
        return -1;
    }

    cout << "Anagram Validator Test: Normal\n";
    auto start1 = chrono::system_clock::now();
    testAnagramNormal(str1, str2);
    auto stop1 = chrono::system_clock::now();
    auto totalTime = chrono::duration_cast<chrono::milliseconds>(stop1 - start1).count();
    cout << "Execution Time for Anagram Validator (NORMAL): " << totalTime << " milliseconds\n";


    // auto start2 = chrono::system_clock::now();
    // cout << "Anagram Validator Test: Parallel\n";
    // testAnagramParallel(str1, str2);
    // auto stop2 = chrono::system_clock::now();
    // auto totalTime2 = chrono::duration_cast<chrono::milliseconds>(stop2 - start2).count();
    // cout << "Execution Time for Anagram Validator (PARLLEL): " << totalTime2 << " microseconds\n";

    return 0;
}
