#include <cmath>
#include <algorithm>

#include "fruchterman_reingold.hpp"

namespace nodesoup {
using namespace std;

void FruchtermanReingold::operator()(vector<Point2D>& positions) {
    Vector2D zero = { 0.0, 0.0 };
    fill(mvmts_.begin(), mvmts_.end(), zero);

    // Force de repulsion entre chaque paire de vertices
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
    }

    // Force d'attraction entre chaque edge
    // (TODO deplacer dans la boucle precedente?)
    for (vertex_id_type v_id = 0; v_id < g_.size(); v_id++) {
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

    /* Mouvement max limite par la temperature */
    for (vertex_id_type v_id = 0; v_id < g_.size(); v_id++) {
        double mvmt_norm = mvmts_[v_id].norm();
        if (mvmt_norm == 0.0) {
            continue;
        }
        double capped_mvmt_norm = min(mvmt_norm, temp_);
        Vector2D capped_mvmt = mvmts_[v_id] / mvmt_norm * capped_mvmt_norm;

        positions[v_id] += capped_mvmt;
    }

    // cool
    /* Descente rapide de la temperature */
    if (temp_ > 1.0) {
        temp_ *= 0.95;
    }
    /* Puis maintien a un niveau bas */
}
}
