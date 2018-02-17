#include "nodesoup.hpp"
#include "fruchterman_reingold.hpp"
#include "kamada_kawai.hpp"
#include "layout.hpp"

namespace nodesoup {
using namespace std;

vector<Point2D> fruchterman_reingold(adj_list_type& g, unsigned int width, unsigned int height, unsigned int iters_count, iter_callback_type iter_cb) {
    // Disposition initiale en cercle
    vector<Point2D> positions(g.size());
    circle(g, positions);

    FruchtermanReingold fr(g);
    for (unsigned int i = 0; i < iters_count; i++) {
        fr(positions);

        if (iter_cb != nullptr) {
            center_and_scale(g, width, height, positions);
            iter_cb(positions, i);
        }
    }

    center_and_scale(g, width, height, positions);
    return positions;
}

vector<Point2D> kamada_kawai(adj_list_type& g, unsigned int width, unsigned int height) {
    // Disposition initiale en cercle
    vector<Point2D> positions(g.size());
    circle(g, positions);

    //center_and_scale(g, WIDTH, HEIGHT, positions);
    KamadaKawai kk(g, width, height);
    kk(positions);
    center_and_scale(g, width, height, positions);

    return positions;
}
}
