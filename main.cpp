// Copyright (c) Pacheco. All rights reserved.
// cls && g++ main.cpp -O3 -o main.exe && main.exe && del main.exe
#include <sys/stat.h>
#include "bfo.cpp"
#include <iostream>
#include <time.h>
#include <chrono>
#include <string>
#include <fstream>

using namespace std;
using namespace chrono;

struct MyTest {
    string file_name;
    string csv_name;
    double infect;
    double recover;
    int bactSize;
    int moves;
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
    params.NC = test.moves * 3;
    params.NS = test.moves;
    params.NED = test.moves;
    params.NRE = 10;
    params.PED = 0.2;
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
        to_string(params.NC)
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

string createTestFileName(string file_name) {
    ofstream outfile (file_name);
    outfile.close();
    return file_name;
}

bool verify_line(string file_name, string line) {
    bool ret = false;
    ifstream file(file_name);
    string str; 
    while (std::getline(file, str))
    {
        if (str.find(line) != string::npos) {
            ret = true;
            break;
        } 
    }
    file.close();
    return ret;
}

inline bool file_exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int main() {
    int type = 0;
    string file_name = "";
    if (type == 0) {
        file_name = ".\\test_home.csv";
    }
    if (type == 1) {
        file_name = ".\\test_work_1.csv";
    }
    if (type == 2) {
        file_name = ".\\test_work_2.csv";
    }
    if (file_exists(file_name) == false){
        createTestFileName(file_name);
        writeFileLine(file_name, "iteracao,instancia,num. bacteria,iter. movimento,iter. chemo,prob. infect, prob. recover, prob. elim,dias,arestas,vertices,tempo,S,I,R");
    }
    vector<string> tests = {
        ".\\datasets\\sx-mathoverflow.txt",
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
    vector<vector<double>> infect;
    vector<int> bactSize = {50, 100};
    vector<int> moves = {5, 10};
    vector<MyTest> tests_bfo;
    if (type == 0) {
        infect = {{0.3, 0.3}, {0.3, 0.5}, {0.3, 0.8}, {0.5, 0.3}, {0.5, 0.8}, {0.8, 0.3}, {0.8, 0.5}, {0.8, 0.8}};
    }
    if (type == 1) {
        infect = {{0.125, 0.125}, {0.34, 0.34}, {0.5, 0.5}, {0.05, 0.05}, {0.15, 0.15}, {0.25, 0.25}, {0.9, 0.9}, {0.014, 0.05}};
        moves = {5};
    }
    if (type == 2) {
        infect = {{0.125, 0.125}, {0.34, 0.34}, {0.5, 0.5}, {0.05, 0.05}, {0.15, 0.15}, {0.25, 0.25}, {0.9, 0.9}, {0.014, 0.05}};
        moves = {10};
    }
    for (vector<double> i : infect) {
        for (int b : bactSize) {
            for (int m : moves) {
                for (string name : tests) {
                    for (int iter = 0; iter < 30; iter++) {
                        MyTest t;
                        t.file_name = name;
                        t.csv_name = file_name;
                        t.infect = i[0];
                        t.recover = i[1];
                        t.bactSize = b;
                        t.moves = m;
                        t.iter_test = iter + 1;
                        tests_bfo.push_back(t);
                    }
                }
            }
        }
    }
    bool validate = true;
    for (vector<MyTest>::iterator itr = tests_bfo.begin(); itr != tests_bfo.end(); itr++) {
        validate = verify_line(file_name, 
            to_string((*itr).iter_test)
            +","+
            (*itr).file_name
            +","+
            to_string((*itr).bactSize)
            +","+
            to_string((*itr).moves)
            +","+
            to_string((*itr).moves * 3)
            +","+
            to_string((*itr).infect)
            +","+
            to_string((*itr).recover)
            +","+
            to_string(0.2)
        );
        if (validate == false) {
            runTest(*itr);
        }
    }
    
    return 0;
}


// em todas considera a melhor melhora após 1000 iterações sem melhora e rodam até 100 dias
// maquina 1
// config dos trabalhos no SIR 
//     qtde_bact                  = {50, 100};
//     qtde_chemotaxia            = 15;
//     qtde_movimentos_elimdisper = 5;
//     qtde_reproducao_bact       = 10;
//     taxa_eliminacao_dispercao  = 0.20;
// maquina 2
// config dos trabalhos no SIR 
//     qtde_bact                  = {50, 100};
//     qtde_chemotaxia            = 30;
//     qtde_movimentos_elimdisper = 10;
//     qtde_reproducao_bact       = 10;
//     taxa_eliminacao_dispercao  = 0.20;
// config dos trabalhos no SIR {infect, recover}
//     {0.125, 0.125}, {0.34, 0.34}, {0.5, 0.5}, {0.05, 0.05}, {0.15, 0.15}, {0.25, 0.25}, {0.9, 0.9}, {0.014, 0.05}
// config casa
//     {0.3, 0.3}, {0.3, 0.5}, {0.3, 0.8}, {0.5, 0.3}, {0.5, 0.8}, {0.8, 0.3}, {0.8, 0.5}, {0.8, 0.8}
