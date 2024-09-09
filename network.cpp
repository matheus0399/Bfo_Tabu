// status
// s - '0' | i - '1' | r - '2' 
// tag
// vector<char>  -> n max size
// edges
// vector<int> -> n neighbors
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

struct NetworkElement {
  char status;
  vector<char> tag;
  vector<int> edges;
};

struct Network {
    vector<NetworkElement> individuals;
    int size;
    int s;
    int i;
    int r;
};

NetworkElement create_element(char status, vector<char> tag) {
    NetworkElement element;
    element.tag = tag;
    element.status = status;
    return element;
}

int exists(vector<NetworkElement> el_vect, vector<char> tag) {
    for (int i = 0; i < el_vect.size(); i++) {
        if (tag == el_vect[i].tag) {
            return i;
        }
    }
    return -1;
}

int exists_addr(vector<int> edges, int index) {
    for (int i = 0; i < edges.size(); i++) {
        if (index == edges[i]) {
            return i;
        }
    }
    return -1;
}

Network create_network(string _path) {
    Network net;
    int edges = 0;
    ifstream file(_path);
    string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.empty()) continue;

            istringstream iss(line);
            string vertex1, vertex2;
            iss >> vertex1 >> vertex2;
            if (vertex1.empty() || vertex2.empty()) continue;
            vector<char> name;
            for (char i: vertex1) name.push_back(i);
            vector<char> name2;
            for (char i: vertex2) name2.push_back(i);
            
            // Check and add vertex1
            int el_index = exists(net.individuals, name);
            if (el_index == -1) {
                net.individuals.push_back(create_element('0', name));
                el_index = net.individuals.size() - 1;
            }

            // Check and add vertex2
            int el2_index = exists(net.individuals, name2);
            if (el2_index == -1) {
                net.individuals.push_back(create_element('0', name2));
                el2_index = net.individuals.size() - 1;
            }
            
            NetworkElement &el = net.individuals[el_index];
            NetworkElement &el2 = net.individuals[el2_index];
            
            // Add edges if any is new
            if (exists_addr(el.edges, el2_index) == -1) {
                edges++;
                el.edges.push_back(el2_index);
                el2.edges.push_back(el_index);
            }
        }
        file.close();
    }
    net.size = net.individuals.size();
    net.s = net.individuals.size();
    net.i = 0;
    net.r = 0;
    return net;
}