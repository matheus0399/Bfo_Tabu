#include "network.cpp"
#include "utils.cpp"
#include <iostream>
#include <time.h>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

void update(Network* net, double infect_percent, double recover_percent) {
    bool infect = false;
    vector<int>::iterator itr;
    for(int i=0; i<net->individuals.size(); ++i) {
        switch(net->individuals.at(i).status) {
            case '0':
                infect = false;
                for (itr = net->individuals.at(i).edges.begin(); itr != net->individuals.at(i).edges.end(); itr++) {
                    if (net->individuals[*itr].status == '1' && randomDouble(0, 1) <= infect_percent) {
                        infect = true;
                        break;
                    }
                }
                if (infect) {
                    net->i += 1;
                    net->s -= 1;
                    net->individuals.at(i).status = '1';
                }
                break;
            case '1':
                if (randomDouble(0, 1) <= recover_percent) {
                    net->individuals[i].status = '2';
                    net->r += 1;
                    net->i -= 1;
                }
                break;
            default:
                break;
        }
    }
}