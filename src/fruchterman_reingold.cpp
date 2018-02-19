#include <algorithm>
#include <cmath>
#include <iostream>

#include "fruchterman_reingold.hpp"

namespace nodesoup {
using namespace std;

FruchtermanReingold::FruchtermanReingold(const adj_list_type& g, double k) :
    g_(g), k_(k), k_squared_(k * k),
    temp_(10 * sqrt(g.size())), mvmts_(g_.size()) {
    build_edges_();
}

void FruchtermanReingold::build_edges_() {
    for (vertex_id_type v_id = 0; v_id < g_.size(); v_id++) {
        for (vertex_id_type adj_id : g_[v_id]) {
            if (adj_id > v_id) {
                continue;
            }

            edges_.push_back({ v_id, adj_id });
        }
    }
}

bool FruchtermanReingold::has_intersecting_edges_(vector<Point2D>& positions) {
    auto ccw = [](Point2D a, Point2D b, Point2D c) -> bool {
        return ((c.y - a.y) * (b.x - a.x)) > ((b.y - a.y) * (c.x - a.x));
    };

    for (size_t i = 0; i < edges_.size(); i++) {
        pair<vertex_id_type, vertex_id_type> first_edge = edges_[i];
        for (size_t j = i + 1; j < edges_.size(); j++) {
            pair<vertex_id_type, vertex_id_type> sec_edge = edges_[j];

            Point2D first_from = positions[first_edge.first];
            Point2D first_to = positions[first_edge.second];
            Point2D sec_from = positions[sec_edge.first];
            Point2D sec_to = positions[sec_edge.second];

            if (ccw(first_from, sec_from, sec_to) != ccw(first_to, sec_from, sec_to) &&
                ccw(first_from, first_to, sec_from) != ccw(first_from, first_to, sec_to)) {
                return true;
            }
        }
    }

    return false;
}

void FruchtermanReingold::operator()(vector<Point2D>& positions) {
    Vector2D zero = { 0.0, 0.0 };
    fill(mvmts_.begin(), mvmts_.end(), zero);

    // Repulsion force between each vertice pair
    for (vertex_id_type v_id = 0; v_id < g_.size(); v_id++) {
        for (vertex_id_type other_id = v_id + 1; other_id < g_.size(); other_id++) {
            if (v_id == other_id) {
                continue;
            }

            Vector2D delta = positions[v_id] - positions[other_id];
            double distance = delta.norm();
            double repulsion = k_squared_ / distance;

            mvmts_[v_id] += delta / distance * repulsion;
            mvmts_[other_id] -= delta / distance * repulsion;
        }

        // Attraction force between each edge
        for (vertex_id_type adj_id : g_[v_id]) {
            if (adj_id > v_id) {
                continue;
            }

            Vector2D delta = positions[v_id] - positions[adj_id];
            double distance = delta.norm();
            double attraction = distance * distance / k_;

            mvmts_[v_id] -= delta / distance * attraction;
            mvmts_[adj_id] += delta / distance * attraction;
        }
    }

    // Max movement capped by current temperature
    for (vertex_id_type v_id = 0; v_id < g_.size(); v_id++) {
        double mvmt_norm = mvmts_[v_id].norm();
        if (mvmt_norm == 0.0) {
            continue;
        }
        double capped_mvmt_norm = min(mvmt_norm, temp_);
        Vector2D capped_mvmt = mvmts_[v_id] / mvmt_norm * capped_mvmt_norm;

        positions[v_id] += capped_mvmt;
    }

    // Cool down fast until we reach 1.0, then stay at low temperature
    if (temp_ > 1.5) {
        temp_ *= 0.85;
    } else {
        temp_ = 2.0;
    }

    cout << (has_intersecting_edges_(positions) ? "true" : "false") << endl;
}
}
