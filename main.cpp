// Copyright (c) Pacheco. All rights reserved.
// cls & g++ main.cpp -o main.exe & main.exe
#include <iostream>
#include <time.h>
#include "network.cpp"

using namespace std;

int main() {
    Network n = create_network(".\\datasets\\CA-GrQc.txt");
    string tag(n.individuals[0].tag.begin(), n.individuals[0].tag.end());
    cout << "indices dos vertices conectados ao vertice " << tag << ":" << endl;
    for (int i : n.individuals[0].edges) {
        cout << i << ", ";
    }
    cout << endl;
    return 0;
}