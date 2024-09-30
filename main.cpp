// Copyright (c) Pacheco. All rights reserved.
// cls && g++ main.cpp -O2 -o main.exe && main.exe && del main.exe
#include "bfo.cpp"
#include <iostream>
#include <time.h>
#include <chrono>

using namespace std;
using namespace chrono;

struct MyTest {
    string file_name;
    string csv_name;
    double infect;
    double recover;
    int bactSize;
    int moves;
    double elim;
    int iter_test;
};

void writeFileLine(string file_name, string line) {
    ofstream myfile;
    myfile.open(file_name, ios::app);
    myfile << line + "\n";
    myfile.close();
}

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

void runTest(MyTest test) {
    auto start = high_resolution_clock::now();
    vector<TabuElement> tabu;
    BFOAParameters params;
    params.S = test.bactSize;
    params.NC = test.moves;
    params.NS = test.moves;
    params.NED = test.moves;
    params.NRE = 50;
    params.PED = test.elim;
    Network n = create_network(test.file_name);
    int amount = n.individuals.size() / 100 * 10; // 10 por cento da população começa infectada
    vector<int> indexes;
    while (indexes.size() != amount) {
        int val = gen_rand_int(0, n.individuals.size() - 1);
        auto it = find(indexes.begin(), indexes.end(), val);
        if (it == indexes.end()) {
            n.individuals[val].status = '1';
            indexes.push_back(val);
        }
    }
    n.s -= amount;
    n.i += amount;
    unordered_set<TabuElement> current;
    unordered_set<TabuElement> last;
    int iter;
    int check;
    int i;
    for (i = 0; i < 100; i++) {
        last.clear();
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
        update(&n, test.infect, test.recover);
        update_tabu(&n, &tabu);
        if (n.i == 0) {
            break;
        }
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    writeFileLine(test.csv_name,
        to_string(test.iter_test)
        +","+
        test.file_name
        +","+
        to_string(params.S)
        +","+
        to_string(test.moves)
        +","+
        to_string(test.infect)
        +","+
        to_string(test.recover)
        +","+
        to_string(params.PED)
        +","+
        to_string(i + 1)
        +","+
        to_string(n.all_edges)
        +","+
        to_string(n.individuals.size())
        +","+
        to_string(duration.count() / 1000)
        +","+
        to_string(n.s)
        +","+
        to_string(n.i)
        +","+
        to_string(n.r));
}

string createTestFileName() {
    auto start = system_clock::now();
    time_t start_to_parse = system_clock::to_time_t(start);
    string ha = ctime(&start_to_parse);
    while (ha.find("\n") != string::npos) {
        size_t start_pos = ha.find("\n");
        ha.replace(start_pos, 1, "");
    }
    while (ha.find(":") != string::npos) {
        size_t start_pos = ha.find(":");
        ha.replace(start_pos, 1, "_");
    }
    while (ha.find(" ") != string::npos) {
        size_t start_pos = ha.find(" ");
        ha.replace(start_pos, 1, "_");
    }
    string file_name = ".\\" + ha + ".csv";
    ofstream outfile (file_name);
    outfile.close();
    return file_name;
}

int main() {
    string file_name = createTestFileName();
    writeFileLine(file_name, "iteracao,instancia,num. bacteria,iter. movimento,prob. infect, prob. recover, prob. elim,dias,arestas,vertices,tempo,S,I,R");
    vector<string> tests = {
        // ".\\datasets\\sx-mathoverflow.txt",
        ".\\datasets\\CA-GrQc.txt",
        ".\\datasets\\CA-HepTh.txt",
        ".\\datasets\\CollegeMsg.txt",
        ".\\datasets\\facebook_combined.txt",
        ".\\datasets\\lastfm_asia_edges.txt",
        ".\\datasets\\p2p-Gnutella08.txt",
        ".\\datasets\\soc-sign-bitcoinalpha.txt",
        ".\\datasets\\soc-sign-bitcoinotc.txt",
        ".\\datasets\\Wiki-Vote.txt",
    };
    vector<double> infect = {0.3, 0.5, 0.8};
    vector<double> recover = {0.3, 0.5, 0.8};
    vector<int> bactSize = {50, 100};
    vector<int> moves = {5, 10};
    vector<double> elim = {0.20, 0.80};
    vector<MyTest> tests_bfo;
    for (double i : infect) {
        for (double r : recover) {
            for (int b : bactSize) {
                for (int m : moves) {
                    for (double e : elim) {
                        for (string name : tests) {
                            for (int iter = 0; iter < 30; iter++) {
                                MyTest t;
                                t.file_name = name;
                                t.csv_name = file_name;
                                t.infect = i;
                                t.recover = r;
                                t.bactSize = b;
                                t.moves = m;
                                t.elim = e;
                                t.iter_test = iter + 1;
                                tests_bfo.push_back(t);
                            }
                        }
                    }
                }
            }
        }
    }
    for (vector<MyTest>::iterator itr = tests_bfo.begin(); itr != tests_bfo.end(); itr++) {
        runTest(*itr);
    }
    return 0;
}


// •	Probabilidade de infecção β = {0.3, 0.5, 0.8};
// •	Probabilidade de remoção γ = {0.3, 0.5, 0.8};
// •	Número de bactérias S = {50, 100};
// •	Número de iterações na movimentação das bactérias NC, NS, NED = {5, 10};
// •	Probabilidade de eliminação das bactérias PED = {0.20, 0.80};

