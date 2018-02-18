#pragma once

#include <vector>

#include "geometry.hpp"
#include "graph.hpp"

namespace nodesoup {
/** Distribute vertices equally on a 1.0 radius circle */
void circle(const adj_list_type& g, std::vector<Point2D>& positions);
/** Center and scale vertices so the graph fits on a canvas of given dimensions */
void center_and_scale(const adj_list_type& g, unsigned int width, unsigned int height, std::vector<Point2D>& positions);
}
