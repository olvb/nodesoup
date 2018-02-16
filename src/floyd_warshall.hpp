#pragma once
#include <vector>

#include "graph.hpp"

std::vector<std::vector<unsigned int>> floyd_warshall(adj_list_type& g);
