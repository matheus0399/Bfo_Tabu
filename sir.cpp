#include "network.cpp"
#include "utils.cpp"
#include <iostream>
#include <time.h>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

void update(Network &net) {
    unordered_set<int>::iterator itr;
    bool infect = false;
    for(int i=0; i<net.individuals.size(); ++i) {
        switch(net.individuals.at(i).status) {
            case '0':
                infect = false;
                for (itr = net.individuals.at(i).edges.begin(); itr != net.individuals.at(i).edges.end(); itr++) {
                    if (net.individuals[*itr].status == '1' && gen_rand_int(1,10) <= 2) {
                        infect = true;
                        break;
                    }
                }
                if (infect) {
                    net.i += 1;
                    net.s -= 1;
                    net.individuals.at(i).status = '1';
                }
                break;
            case '1':
                if (gen_rand_int(1, 10) <= 5) {
                    net.individuals[i].status = '2';
                    net.r += 1;
                    net.i -= 1;
                }
                break;
            default:
                break;
        }
    }
}