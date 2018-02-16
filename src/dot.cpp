#include <algorithm>
#include <cassert>
#include <sstream>

#include "dot.hpp"

using namespace std;

adj_list_type read_from_dot(string dot) {

    // retourne name sans le 1er caractere
    // ex: v20 -> 20
    auto name_to_vertex_id = [](string name) -> vertex_id_type {
        assert(name[0] == 'v');
        istringstream iss(name.substr(1));
        vertex_id_type v_id;
        iss >> v_id;
        return v_id;
    };

    adj_list_type g;
    string line;
    istringstream iss(dot);
    while (std::getline(iss, line)) {
        if (line[0] != 'v') {
            continue;
        }

        istringstream iss(line);
        string name, edge_sign, adj_name;
        iss >> name >> edge_sign >> adj_name;

        vertex_id_type v_id = name_to_vertex_id(name);
        // add vertex (and preceding vertices) if new
        if (v_id >= g.size()) {
            g.resize(v_id + 1);
            assert(v_id < g.size());
        }

        assert(edge_sign == "--" || edge_sign.size() == 0);
        if (edge_sign != "--") {
            continue;
        }

        vertex_id_type adj_id = name_to_vertex_id(adj_name);

        // add adjacent vertex (and preceding vertices) if new
        if (adj_id >= g.size()) {
            g.resize(adj_id + 1);
            assert(adj_id < g.size());
        }

        // add edge if new
        auto it = find(g[v_id].begin(), g[v_id].end(), adj_id);
        if (it == g[v_id].end()) {
            g[v_id].push_back(adj_id);
            g[adj_id].push_back(v_id);
        }
    }

    return g;
}
