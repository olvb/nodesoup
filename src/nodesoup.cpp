#include "nodesoup.hpp"
#include "fruchterman_reingold.hpp"
#include "kamada_kawai.hpp"
#include "layout.hpp"

namespace nodesoup {
using namespace std;

vector<Point2D> fruchterman_reingold(
    const adj_list_type& g,
    unsigned int width,
    unsigned int height,
    unsigned int iters_count,
    double k,
    iter_callback_type iter_cb) {
    vector<Point2D> positions(g.size());
    // Initial layout on a circle
    circle(g, positions);

    FruchtermanReingold fr(g, k);
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

vector<Point2D> kamada_kawai(
    const adj_list_type& g,
    unsigned int width,
    unsigned int height,
    double k,
    double energy_threshold) {
    vector<Point2D> positions(g.size());
    // Initial layout on a circle
    circle(g, positions);
    KamadaKawai kk(g, k, energy_threshold);
    kk(positions);
    center_and_scale(g, width, height, positions);

    return positions;
}
}
