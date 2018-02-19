#pragma once

#include <utility>
#include <vector>

#include "geometry.hpp"
#include "graph.hpp"

namespace nodesoup {
class FruchtermanReingold {
public:
    FruchtermanReingold(const adj_list_type& g, double k);
    void operator()(std::vector<Point2D>& positions);

private:
    const adj_list_type& g_;
    const double k_;
    const double k_squared_;
    double temp_;
    std::vector<std::pair<vertex_id_type, vertex_id_type>> edges_;
    std::vector<Vector2D> mvmts_;

    void build_edges_();
    bool has_intersecting_edges_(std::vector<Point2D>& positions);
};
}
