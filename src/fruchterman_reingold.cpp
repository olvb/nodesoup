#include <cmath>

#include "fruchterman_reingold.hpp"

using namespace std;

void FruchtermanReingold::operator()(vector<Point2D>& positions) {
    /* Force de repulsion entre chaque paire de vertices */
    for (vertex_id_type v_id = 0; v_id < g_.size(); v_id++) {
        for (vertex_id_type other_id = 0; other_id < g_.size(); other_id++) {
            if (v_id == other_id) {
                continue;
            }

            double x_delta = positions[v_id].x - positions[other_id].x;
            double y_delta = positions[v_id].y - positions[other_id].y;

            double distance = sqrt(x_delta * x_delta + y_delta * y_delta);

            double repulsion = k_ * k_ / distance;

            mvmts_[v_id].dx += x_delta / distance * repulsion;
            mvmts_[v_id].dy += y_delta / distance * repulsion;
            /* TODO traiter other_id en meme temps? */
        }
    }

    /* Force d'attraction entre chaque edge */
    /* (TODO deplacer dans la boucle precedente?) */
    for (vertex_id_type v_id = 0; v_id < g_.size(); v_id++) {
        for (vertex_id_type adj_id : g_[v_id]) {
            if (adj_id > v_id) {
                continue;
            }

            double x_delta = positions[v_id].x - positions[adj_id].x;
            double y_delta = positions[v_id].y - positions[adj_id].y;
            double distance = sqrt(x_delta * x_delta + y_delta * y_delta);

            double attraction = distance * distance / k_;

            mvmts_[v_id].dx -= x_delta / distance * attraction;
            mvmts_[v_id].dy -= y_delta / distance * attraction;

            mvmts_[adj_id].dx += x_delta / distance * attraction;
            mvmts_[adj_id].dy += y_delta / distance * attraction;
        }
    }

    for (vertex_id_type v_id = 0; v_id < g_.size(); v_id++) {
        double mvmt = sqrt(mvmts_[v_id].dx * mvmts_[v_id].dx + mvmts_[v_id].dy * mvmts_[v_id].dy);
        if (mvmt == 0.0) {
            continue;
        }

        /* Mouvement max limite par la temperature */
        double capped_mvmt = min<double>(mvmt, temp_);
        //double capped_mvmt = mvmt;
        mvmts_[v_id].dx = mvmts_[v_id].dx / mvmt * capped_mvmt;
        mvmts_[v_id].dy = mvmts_[v_id].dy / mvmt * capped_mvmt;

        positions[v_id].x += mvmts_[v_id].dx;
        positions[v_id].y += mvmts_[v_id].dy;

        /* Pour repartir clean a la prochaine iteration */
        mvmts_[v_id] = { 0, 0 };
    }

    // cool
    /* Descente rapide de la temperature */
    if (temp_ > 1.0) {
        temp_ *= 0.95;
    }
    /* Puis maintien a un niveau bas */
}
