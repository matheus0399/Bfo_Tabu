#include <iostream>
#include <time.h>
#include "network.cpp"
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

void interact(NetworkElement* person, Network &net) {
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
    uniform_int_distribution<unsigned> distrib(1,10);
    unordered_set<int>::iterator itr;
    for (itr = person->edges.begin(); itr != person->edges.end(); itr++) {
        if (net.individuals[*itr].status == '0' && distrib(gen) > 5) {
            net.i += 1;
            net.s -= 1;
            net.individuals[*itr].status = '1';
        }
    }
}

void update(Network &net) {
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
    uniform_int_distribution<unsigned> distrib(1,10);

    for(int i=0; i<net.individuals.size(); ++i) {
        NetworkElement* person = &net.individuals.at(i);
        switch(person->status) {
            case '1':
                interact(person, net);
                if (distrib(gen) > 5) {
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