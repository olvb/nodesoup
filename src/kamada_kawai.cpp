#include "kamada_kawai.hpp"
#include "floyd_warshall.hpp"

#include <cassert>
#include <cmath>

using namespace std;

// epsilon in original algorithm , can be adjusted
#define ENERGY_THRESHOLD 1.e-2
#define K 10

KamadaKawai::KamadaKawai(adj_list_type& g, unsigned int width, unsigned int height) :
    g_(g) {
    vector<vector<unsigned int>> distances = floyd_warshall(g_);

    unsigned int biggest_distance = 0;
    for (vertex_id_type v_id = 0; v_id < g_.size(); v_id++) {
        for (vertex_id_type other_id = 0; other_id < g_.size(); other_id++) {
            if (distances[v_id][other_id] > biggest_distance) {
                biggest_distance = distances[v_id][other_id];
            }
        }
    }

    // Ideal length for all edges
    // (we don't really care, the layout is going to be scaled)
    double l = ((width + height) / 2.0) / biggest_distance;

    // init springs lengths and strenghts matrices
    for (vertex_id_type v_id = 0; v_id < g_.size(); ++v_id) {
        vector<Spring> v_springs;

        for (vertex_id_type other_id = 0; other_id < g_.size(); ++other_id) {
            Spring spring;
            if (v_id == other_id) {
                spring.strength = 0.0;
                spring.length = 0.0;
            } else {
                unsigned int distance = distances[v_id][other_id];
                spring.length = distance * l;
                spring.strength = K / (distance * distance);
            }

            v_springs.push_back(spring);
        }

        springs_.push_back(v_springs);
    }
}

/**
Reduce the energy of the next vertex with most energy until all the vertices have
a energy below ENERGY_THRESHOLD
*/
void KamadaKawai::operator()(vector<Point2D>& positions) {
    vertex_id_type v_id;
    while (get_max_vertex_energy_(positions, v_id) > ENERGY_THRESHOLD) {
        // move vertex step by step until its energy goes below threshold
        // (apparently this is equivalent to the newton raphson method)
        unsigned int count = 0;
        do {
            positions[v_id] = get_lesser_energy_position_for_vertex_(v_id, positions);
            count++;
        } while (get_vertex_energy_(v_id, positions) > ENERGY_THRESHOLD && count < 50);
    }
}

/**
Find @p max_energy_v_id with the most potential energy and @return its energy
// https://gist.github.com/terakun/b7eff90c889c1485898ec9256ca9f91d
*/
double KamadaKawai::get_max_vertex_energy_(vector<Point2D>& positions, vertex_id_type& max_energy_v_id) {
    double max_energy = -1.0;
    for (vertex_id_type v_id = 0; v_id < g_.size(); ++v_id) {
        double energy = get_vertex_energy_(v_id, positions);
        if (energy > max_energy) {
            max_energy_v_id = v_id;
            max_energy = energy;
        }
    }
    assert(max_energy != -1.0);
    return max_energy;
}

/** @return the potential energies of springs between @p v_id and all other vertices */
double KamadaKawai::get_vertex_energy_(vertex_id_type v_id, vector<Point2D>& positions) {
    double x_energy = 0.0;
    double y_energy = 0.0;

    for (vertex_id_type other_id = 0; other_id < g_.size(); ++other_id) {
        if (v_id == other_id) {
            continue;
        }

        double x_delta = positions[v_id].x - positions[other_id].x;
        double y_delta = positions[v_id].y - positions[other_id].y;
        double distance = sqrt(x_delta * x_delta + y_delta * y_delta);

        Spring spring = springs_[v_id][other_id];
        double k = spring.strength;
        double l = spring.length;
        x_energy += x_delta * k * (1.0 - l / distance);
        y_energy += y_delta * k * (1.0 - l / distance);
    }

    return sqrt(x_energy * x_energy + y_energy * y_energy);
}

/**
@returns next position for @param v_id reducing its potential energy, ie the energy in the whole graph
caused by its position.
The position's delta depends on K (TODO bigger K = faster?).
This is the complicated part of the algorithm.
*/
Point2D KamadaKawai::get_lesser_energy_position_for_vertex_(vertex_id_type v_id, vector<Point2D>& positions) {
    // Calculs d'energies avec valeurs differences pour x et y
    double xx_energy = 0.0, xy_energy = 0.0, yx_energy = 0.0, yy_energy = 0.0;
    double x_energy = 0.0, y_energy = 0.0;

    for (vertex_id_type other_id = 0; other_id < g_.size(); ++other_id) {
        if (v_id == other_id) {
            continue;
        }

        Spring spring = springs_[v_id][other_id];
        // strengh between 2 vertices
        double k = spring.strength;
        // length of path between 2 vertices
        double l = spring.length;

        // positional deltas
        double x_delta = positions[v_id].x - positions[other_id].x;
        double y_delta = positions[v_id].y - positions[other_id].y;

        // cubed euclidean distance
        double distance = sqrt(x_delta * x_delta + y_delta * y_delta);
        double cubed_distance = distance * distance * distance;

        x_energy += x_delta * k * (1.0 - l / distance);
        y_energy += y_delta * k * (1.0 - l / distance);

        xy_energy += k * l * x_delta * y_delta / cubed_distance;
        // pas compris pourquoi mais les 2 sources ont l air d accord
        // (meme si y en a un qui a du copier sur l autre)
        xx_energy += k * (1.0 - l * y_delta * y_delta / cubed_distance);
        yy_energy += k * (1.0 - l * x_delta * x_delta / cubed_distance);
    }

    yx_energy = xy_energy;

    Point2D position = positions[v_id];
    double denom = xx_energy * yy_energy - xy_energy * yx_energy;
    position.x += (xy_energy * y_energy - yy_energy * x_energy) / denom;
    position.y += (xy_energy * x_energy - xx_energy * y_energy) / denom;

    return position;
}
