#pragma once
#include <cmath>
#include <limits>

#include "floyd_warshall.hpp"

using namespace std;

vector<vector<unsigned int>> floyd_warshall(adj_list_type& g) {
    unsigned int infinity = std::numeric_limits<unsigned int>::max() / 2;
    vector<vector<unsigned int>> distances(g.size(), vector<unsigned int>(g.size(), infinity));

    for (vertex_id_type v_id = 0; v_id < g.size(); v_id++) {
        distances[v_id][v_id] = 0;
        for (vertex_id_type adj_id : g[v_id]) {
            distances[v_id][adj_id] = 1;
            distances[adj_id][v_id] = 1;
        }
    }

    for (unsigned int k = 0; k < g.size(); k++) {
        for (unsigned int i = 0; i < g.size(); i++) {
            for (unsigned int j = 0; j < g.size(); j++) {
                distances[i][j] = std::min(distances[i][j], distances[i][k] + distances[k][j]);
            }
        }
    }

    return distances;
}
