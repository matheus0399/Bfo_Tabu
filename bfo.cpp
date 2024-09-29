// TO DO
#include "sir.cpp"
#include <unordered_set>
#include <algorithm>
#include <pthread.h>

struct BFOAParameters {
    int S;         // O número de bactérias
    int NC;        // O número de iterações quimiotáticas
    int NS;        // O número de iterações de movimentação das bactérias
    int NRE;       // O número de iterações de reprodução das bactérias
    int NED;       // O número de iterações de eliminação/dispersão das bactérias
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

void chemotaxis(vector<Bacteria>* bacteria_population, BFOAParameters* params, Network* net, unordered_set<TabuElement>* last_best) {
    for (int j = 0; j < params->NC; j++) {
        for (
            vector<Bacteria>::iterator it = (*bacteria_population).begin();
            net->individuals[(*it).pos].edges.empty() == false && it != (*bacteria_population).end();
            ++it
        ) {
            int random_index = gen_rand_int(0, net->individuals[(*it).pos].edges.size() - 1);
            int newPos = net->individuals[(*it).pos].edges[random_index];
            int newFit = (*it).fit;
            if (
                (net->individuals[(*it).pos].status == '0' &&  net->individuals[newPos].status == '1') ||
                (net->individuals[(*it).pos].status == '1' &&  net->individuals[newPos].status == '0')
            ) {
                newFit += 1;
                TabuElement i_1;
                i_1.index_1 = newPos;
                i_1.index_2 = (*it).pos;
                if ((*it).pos < newPos) {
                    i_1.index_1 = (*it).pos;
                    i_1.index_2 = newPos;
                }
                i_1.time = 0;
                (*last_best).insert(i_1);
                (*it).fit = newFit;
            }
            (*it).pos = newPos;
        }
    }
}

void reproduction(vector<Bacteria>* bacteria_population) {
    sort((*bacteria_population).begin(), (*bacteria_population).end(), [](Bacteria a, Bacteria b) {
        return a.fit > b.fit;
    });
    copy((*bacteria_population).begin(), (*bacteria_population).begin() + (*bacteria_population).size() / 2,
        (*bacteria_population).begin() + (*bacteria_population).size() / 2);
}

void eliminationDispersal(vector<Bacteria>* bacteria_population, BFOAParameters* params, Network* net) {
    for (vector<Bacteria>::iterator it = (*bacteria_population).begin(); it != (*bacteria_population).end(); ++it) {
        if (randomDouble(0, 1) < params->PED) {
            (*it).pos = gen_rand_int(0, net->individuals.size() - 1);
        }
    }
}

unordered_set<TabuElement> bfo(Network* net, BFOAParameters* params) {
    unordered_set<TabuElement> last_best;
    vector<Bacteria> bacteria_population;
    for (int i = 0; i < params->S; i++) {
        Bacteria b;
        b.pos = gen_rand_int(0, net->individuals.size() - 1);
        b.fit = 0;
        bacteria_population.push_back(b);
    }
    for (int ell = 0; ell < params->NED; ell++) {
        for (int k = 0; k < params->NRE; k++) {
            chemotaxis(&bacteria_population, params, net, &last_best);
            reproduction(&bacteria_population);
        }
        eliminationDispersal(&bacteria_population, params, net);
    }
    return last_best;
}