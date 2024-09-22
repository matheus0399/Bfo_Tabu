#include <iostream>
#include <time.h>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

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

int gen_rand_int(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

double randomDouble(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(gen);
}