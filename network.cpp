// status
// s - '0' | i - '1' | r - '2' 
// tag
// vector<char>  -> n max size
// edges
// vector<int> -> n neighbors
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct NetworkElement {
    char status;
    vector<char> tag;
    vector<int> edges;
};

struct Network {
    vector<NetworkElement> individuals;
    int all_edges;
    int s;
    int i;
    int r;
};

NetworkElement create_element(char status, const vector<char>& tag) {
    NetworkElement element;
    element.tag = tag;
    element.status = status;
    return element;
}

int exists(const unordered_map<string, int>& name_to_index, const vector<char>& tag) {
    string tag_str(tag.begin(), tag.end());
    auto it = name_to_index.find(tag_str);
    return (it != name_to_index.end()) ? it->second : -1;
}

Network create_network(const string& _path) {
    Network net;
    unordered_map<string, int> name_to_index;
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

            vector<char> name(vertex1.begin(), vertex1.end());
            vector<char> name2(vertex2.begin(), vertex2.end());

            // Check and add vertex1
            int el_index = exists(name_to_index, name);
            if (el_index == -1) {
                el_index = net.individuals.size();
                name_to_index[vertex1] = el_index;
                net.individuals.push_back(create_element('0', name));
            }

            // Check and add vertex2
            int el2_index = exists(name_to_index, name2);
            if (el2_index == -1) {
                el2_index = net.individuals.size();
                name_to_index[vertex2] = el2_index;
                net.individuals.push_back(create_element('0', name2));
            }

            // Add edges if any is new
            if (find(net.individuals[el_index].edges.begin(), net.individuals[el_index].edges.end(), el2_index) == net.individuals[el_index].edges.end()) {
                edges++;
                net.individuals[el_index].edges.push_back(el2_index);
                net.individuals[el2_index].edges.push_back(el_index);
            }
        }
        file.close();
    }
    net.all_edges = edges;
    net.s = net.individuals.size();
    net.i = 0;
    net.r = 0;
    return net;
}