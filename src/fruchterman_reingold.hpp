#pragma once

#include <vector>

#include "geometry.hpp"
#include "graph.hpp"

namespace nodesoup {
class FruchtermanReingold {
public:
    FruchtermanReingold(const adj_list_type& g) :
        g_(g), k_(10), k_squared_(10 * 10),
        temp_(10 * sqrt(g.size())), mvmts_(g_.size()) {}
    void operator()(std::vector<Point2D>& positions);

private:
    const adj_list_type& g_;
    const double k_;
    const double k_squared_;
    double temp_;
    std::vector<Vector2D> mvmts_;
};
}
