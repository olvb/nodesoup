#pragma once

#include <vector>

namespace nodesoup {
using vertex_id_type = std::size_t;
using adj_list_type = std::vector<std::vector<vertex_id_type>>;
}
