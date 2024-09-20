// Copyright (c) Pacheco. All rights reserved.
// cls & g++ main.cpp -o main.exe & main.exe
#include <iostream>
#include <time.h>
#include "sir.cpp"
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;

int main() {
    random_device rd;
    mt19937::result_type seed = rd() ^ (
        (mt19937::result_type)duration_cast<seconds>(
            system_clock::now().time_since_epoch()
        ).count() +
        (mt19937::result_type)duration_cast<microseconds>(
            high_resolution_clock::now().time_since_epoch()
        ).count()
    );
    mt19937 gen(seed);
    auto start = high_resolution_clock::now();
    Network n = create_network(".\\datasets\\sx-mathoverflow.txt");
    uniform_int_distribution<unsigned> distrib(0, n.individuals.size() - 1);
    vector<int> indexes;
    while (indexes.size() != 10) {
        int val = distrib(gen);
        auto it = find(indexes.begin(), indexes.end(), val);
        if (it == indexes.end()) {
            indexes.push_back(val);
        }
    }
    for (int i = 0; i < indexes.size(); i++) {
        n.individuals[i].status = '1';
    }
    n.s -= 10;
    n.i += 10;
    cout << "todos: " << n.individuals.size() << endl;
    for (int i = 0; i < 100; i++) {
        update(n);
        cout << "infectados: " << n.i << endl;
        cout << "suscetiveis: " << n.s << endl;
        cout << "recuperados: " << n.r << endl << endl;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by function: " << duration.count() / 1000000 << " seconds" << endl;
    return 0;
}