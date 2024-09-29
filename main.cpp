// Copyright (c) Pacheco. All rights reserved.
// cls & g++ main.cpp -O3 -o main.exe & main.exe & del main.exe
#include "bfo.cpp"
#include <iostream>
#include <time.h>
#include <chrono>

using namespace std;
using namespace chrono;

void add_tabu(Network* n, vector<TabuElement>* tabu, unordered_set<TabuElement>* newElements) {
    for (unordered_set<TabuElement>::iterator itr = (*newElements).begin(); itr != (*newElements).end(); itr++) {
        TabuElement i = *itr;
        n->individuals[i.index_1].edges.erase(find(n->individuals[i.index_1].edges.begin(), n->individuals[i.index_1].edges.end(), i.index_2));
        n->individuals[i.index_2].edges.erase(find(n->individuals[i.index_2].edges.begin(), n->individuals[i.index_2].edges.end(), i.index_1));
        i.time = gen_rand_int(1, 10);
        (*tabu).push_back(i);
    }
}
        
void update_tabu(Network* n, vector<TabuElement>* tabu) {
    vector<TabuElement> delete_indexes;
    for (vector<TabuElement>::iterator itr = (*tabu).begin(); itr != (*tabu).end(); itr++) {
        (*itr).time--;
        if ((*itr).time <= 0) {
            n->individuals[(*itr).index_1].edges.push_back((*itr).index_2);
            n->individuals[(*itr).index_2].edges.push_back((*itr).index_1);
            delete_indexes.push_back(*itr);
        }
    }
    for (vector<TabuElement>::iterator itr = delete_indexes.begin(); itr != delete_indexes.end(); itr++) {
        (*tabu).erase(find((*tabu).begin(),(*tabu).end(), *itr));
    }
}

int main() {
    vector<TabuElement> tabu;
    BFOAParameters params;
    params.S = 100;
    params.NC = 10;
    params.NS = 10;
    params.NRE = 50;
    params.NED = 10;
    params.PED = .5;
    params.C = 1.0;
    auto start = high_resolution_clock::now();
    Network n = create_network(".\\datasets\\sx-mathoverflow.txt");
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
    unordered_set<TabuElement> current;
    unordered_set<TabuElement> last;
    int iter;
    int check;
    for (int i = 0; i < 100; i++) {
        last.clear();
        cout << "loop number " << i << endl;
        iter = 0;
        while (iter < 1000) {
            current = bfo(&n, &params);
            if (last.size() < current.size()) {
                iter = 0;
                last = current;
            } else {
                iter++;
            }
        }
        if (last.empty() == false) {
            add_tabu(&n, &tabu, &last);
        }
        update(&n);
        cout << "arestas na lista tabu: " << tabu.size() << endl;
        update_tabu(&n, &tabu);
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
    return 0;
}