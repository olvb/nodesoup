#include <vector>
#include <functional>

#include "geometry.hpp"
#include "graph.hpp"

namespace nodesoup {
using iter_callback_type = std::function<void(const std::vector<Point2D>&, int)>;
std::vector<Point2D> fruchterman_reingold(adj_list_type& g,  unsigned int width, unsigned int height, unsigned int iters_count = 100, iter_callback_type iter_cb = nullptr);
std::vector<Point2D> kamada_kawai(adj_list_type& g, unsigned int width, unsigned int height);
}
