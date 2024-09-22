// Copyright (c) Pacheco. All rights reserved.
// cls & g++ main.cpp -pthread -o main.exe & main.exe & del main.exe
#include "bfo.cpp"
#include <iostream>
#include <time.h>
#include <chrono>

using namespace std;
using namespace chrono;

void add_tabu(Network& n, vector<TabuElement>& tabu, unordered_set<TabuElement> newElements){
    unordered_set<TabuElement>::iterator itr;
    for (itr = newElements.begin(); itr != newElements.end(); itr++) {
        TabuElement i = *itr;
        n.individuals[i.index_1].edges.erase(n.individuals[i.index_1].edges.find(i.index_2));
        n.individuals[i.index_2].edges.erase(n.individuals[i.index_2].edges.find(i.index_1));
        i.time = gen_rand_int(1, 10);
        tabu.push_back(i);
    }
}
        
void update_tabu(Network& n, vector<TabuElement>& tabu) {
    vector<int> delete_indexes;
    for (int i = 0; i < tabu.size(); i++) {
        tabu[i].time--;
        if (tabu[i].time <= 0) {
            n.individuals[tabu[i].index_1].edges.insert(tabu[i].index_2);
            n.individuals[tabu[i].index_2].edges.insert(tabu[i].index_1);
            delete_indexes.push_back(i);
        }
    }
    sort(delete_indexes.rbegin(), delete_indexes.rend());
    for (int i = 0; i < delete_indexes.size(); i++) {
        if (delete_indexes[i] >= 0 && delete_indexes[i] < tabu.size()) {
            tabu.erase(tabu.begin() + delete_indexes[i]);
        }
    }
}

int main() {
    try {
        vector<TabuElement> tabu;
        BFOAParameters params;
        params.S = 20;
        params.NC = 5;
        params.NS = 5;
        params.NRE = 50;
        params.NED = 5;
        params.PED = .5;
        params.C = 1.0;
        params.last_best = 0;
        auto start = high_resolution_clock::now();
        Network n = create_network(".\\datasets\\CollegeMsg.txt");
        vector<int> indexes;
        while (indexes.size() != 10) {
            int val = gen_rand_int(0, n.individuals.size() - 1);
            auto it = find(indexes.begin(), indexes.end(), val);
            if (it == indexes.end()) {
                n.individuals[val].status = '1';
                indexes.push_back(val);
            }
        }
        n.s -= 10;
        n.i += 10;
        cout << "todos: " << n.individuals.size() << endl;
        int iter;
        int check;
        for (int i = 0; i < 100; i++) {
            iter = 0;
            unordered_set<TabuElement> current;
            unordered_set<TabuElement> last;
            params.last_best = 0;
            while (iter < 100) {
                check = params.last_best;
                current = bfo(n, params);
                if (check < params.last_best) {
                    iter = 0;
                    last = current;
                } else {
                    iter++;
                }
            }
            if (last.empty() == false) {
                add_tabu(n, tabu, last);
            }
            update(n);
            cout << "arestas na lista tabu: " << tabu.size() << endl;
            update_tabu(n, tabu);
            cout << "infectados: " << n.i << endl;
            cout << "suscetiveis: " << n.s << endl;
            cout << "recuperados: " << n.r << endl << endl;
            if (n.i == 0) {
                break;
            }
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by function: " << duration.count() / 1000000 << " seconds" << endl;
    } catch (const std::exception &exc) {
        std::cerr << exc.what();
    }
    return 0;
}