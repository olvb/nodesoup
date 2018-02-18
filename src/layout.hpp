#pragma once

#include <vector>

#include "geometry.hpp"
#include "graph.hpp"

namespace nodesoup {
void circle(const adj_list_type& g, std::vector<Point2D>& positions);
void center_and_scale(const adj_list_type& g, unsigned int width, unsigned int height, std::vector<Point2D>& positions);
}
