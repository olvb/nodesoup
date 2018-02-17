#pragma once

#include <vector>

#include "geometry.hpp"
#include "graph.hpp"

namespace nodesoup {
// https://gist.github.com/terakun/b7eff90c889c1485898ec9256ca9f91d
// https://graphsharp.codeplex.com/SourceControl/latest#Source/Graph#/Algorithms/Layout/Simple/FDP/KKLayoutAlgorithm.cs
class KamadaKawai {
public:
    struct Spring {
        double length;
        double strength;
    };

    KamadaKawai(adj_list_type& g, unsigned int width, unsigned int height);
    void operator()(std::vector<Point2D>& positions);

private:
    adj_list_type& g_;
    std::vector<std::vector<Spring>> springs_;

    static std::vector<std::vector<unsigned int>> floyd_warshall_(adj_list_type& g);
    // p m
    double get_max_vertex_energy_(std::vector<Point2D>& positions, vertex_id_type& max_energy_v_id);
    // delta m
    double get_vertex_energy_(vertex_id_type v_id, std::vector<Point2D>& positions);
    Point2D get_lesser_energy_position_for_vertex_(vertex_id_type v_id, std::vector<Point2D>& positions);
};

}
