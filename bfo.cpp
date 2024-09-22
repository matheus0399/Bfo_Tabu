// TO DO
#include "sir.cpp"
#include <algorithm>
#include <pthread.h>

struct BFOAParameters {
    int S;         // O número de bactérias
    int NC;        // O número de iterações quimiotáticas
    int NS;        // O número de iterações de movimentação das bactérias
    int NRE;       // O número de iterações de reprodução das bactérias
    int NED;       // O número de iterações de eliminação/dispersão das bactérias
    int last_best; // guarda o ultimo melhor valor
    double C;      // valor pra utilizar na função objectiveFunction (ainda precisa validar a utilidade dessa variavel)
    double PED;    // A probabilidade de eliminação de uma bactéria
};

struct Bacteria {
    int pos;
    int fit;
};

struct TabuElement {
    int time;
    int index_1;
    int index_2;
    bool operator==(const TabuElement &other) const {
        return time == other.time && index_1 == other.index_1 && index_2 == other.index_2;
    }
};

struct ChemoArgs {
    Bacteria* bact;
    Network* net;
    unordered_set<TabuElement>* last_best;
};

namespace std {
    template <>
    struct hash<TabuElement> {
        std::size_t operator()(const TabuElement& elem) const noexcept {
            std::size_t seed = 0;
            seed ^= std::hash<int>()(elem.time) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<int>()(elem.index_1) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<int>()(elem.index_2) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}

double objectiveFunction(double x) {
    return x * x;
}

pthread_mutex_t mtx;
void* chemo(void* args) {
    ChemoArgs* chemoArgs = static_cast<ChemoArgs*>(args);
    if (!chemoArgs || !chemoArgs->bact || !chemoArgs->net) {
        return nullptr;
    }
    Bacteria& bact = *(chemoArgs->bact);
    Network& net = *(chemoArgs->net);
    unordered_set<TabuElement>& last_best = *(chemoArgs->last_best);
    if (net.individuals[bact.pos].edges.empty()) {
        return nullptr;
    }
    vector<int> edges_vector(net.individuals[bact.pos].edges.begin(), net.individuals[bact.pos].edges.end());
    int random_index = gen_rand_int(0, edges_vector.size() - 1);
    int newPos = edges_vector[random_index];
    int newFit = bact.fit;
    if (
        (net.individuals[bact.pos].status == '0' && net.individuals[newPos].status == '1') ||
        (net.individuals[bact.pos].status == '1' && net.individuals[newPos].status == '0')
    ) {
        newFit += objectiveFunction(2);
    }
    if (newFit > bact.fit) {
        TabuElement i_1;
        if (bact.pos < newPos) {
            i_1.index_1 = bact.pos;
            i_1.index_2 = newPos;
        } else {
            i_1.index_1 = newPos;
            i_1.index_2 = bact.pos;
        }
        i_1.time = 0;

        pthread_mutex_lock(&mtx);
        if (last_best.find(i_1) == last_best.end()) {
            last_best.insert(i_1);
        }
        pthread_mutex_unlock(&mtx);
        
        bact.pos = newPos;
        bact.fit = newFit;
    }
}

void chemotaxis(vector<Bacteria>& bacteria_population, BFOAParameters params, Network net, unordered_set<TabuElement>& last_best) {
    for (int j = 0; j < params.NC; j++) {
        pthread_mutex_init(&mtx, NULL);
        vector<pthread_t> threads(bacteria_population.size());
        vector<int> threadArgs(bacteria_population.size());
        for (int i = 0; i < bacteria_population.size(); i++) {
            ChemoArgs args = { &bacteria_population[i], &net, &last_best };
            threadArgs[i] = i + 1;
            pthread_create(&threads[i], nullptr, chemo, &args);
        }
        for (int i = 0; i < bacteria_population.size(); ++i) {
            pthread_join(threads[i], nullptr);
        }
        pthread_mutex_destroy(&mtx);
    }
}

void reproduction(vector<Bacteria>& bacteria_population) {
    sort(bacteria_population.begin(), bacteria_population.end(), [](Bacteria a, Bacteria b) {
        return a.fit > b.fit;
    });

    copy(bacteria_population.begin(), bacteria_population.begin() + bacteria_population.size() / 2,
              bacteria_population.begin() + bacteria_population.size() / 2);
}

void eliminationDispersal(vector<Bacteria>& bacteria_population, BFOAParameters params, Network net) {
    for (int i = 0; i < bacteria_population.size(); i++) {
        if (randomDouble(0, 1) < params.PED) {
            bacteria_population[i].pos = gen_rand_int(0, net.individuals.size() - 1);
        }
    }
}

unordered_set<TabuElement> bfo(Network net, BFOAParameters& params) {
    unordered_set<TabuElement> last_best;
    vector<Bacteria> bacteria_population;
    for (int i = 0; i < params.S; i++) {
        Bacteria b;
        b.pos = gen_rand_int(0, net.individuals.size() - 1);
        b.fit = 0.0;
        bacteria_population.push_back(b);
    }
    for (int ell = 0; ell < params.NED; ell++) {
        for (int k = 0; k < params.NRE; k++) {
            chemotaxis(bacteria_population, params, net, last_best);
            reproduction(bacteria_population);
        }
        eliminationDispersal(bacteria_population, params, net);
    }
    if (params.last_best < last_best.size()) {
        params.last_best = last_best.size();
    }
    return last_best;
}