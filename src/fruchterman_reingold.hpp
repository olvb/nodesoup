#pragma once

#include <vector>

#include "geometry.hpp"
#include "graph.hpp"

class FruchtermanReingold {
public:
    FruchtermanReingold(adj_list_type& g) :
        g_(g), k_(10), temp_(10 * sqrt(g.size())), mvmts_(g_.size(), { 0, 0 }) {}
    void operator()(std::vector<Point2D>& positions);

private:
    adj_list_type& g_;
    double k_;
    double temp_;
    std::vector<Vector2D> mvmts_;
};
