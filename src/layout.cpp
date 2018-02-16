#include <cmath>
#include <iostream>
#include <limits>

#include "layout.hpp"

using namespace std;

void circle(adj_list_type& g, vector<Point2D>& positions) {
    double e = 2.0 * M_PI / g.size();
    for (vertex_id_type v_id = 0; v_id < g.size(); v_id++) {
        positions[v_id].x = cos(v_id * e);
        positions[v_id].y = sin(v_id * e);
    }
}

/* agrandir le graphe pour occuper tout l'espace de la fenetre */
void center_and_scale(adj_list_type& g, unsigned int width, unsigned int height, vector<Point2D>& positions) {
    double x_min = numeric_limits<double>::max();
    double x_max = numeric_limits<double>::lowest();
    double y_min = numeric_limits<double>::max();
    double y_max = numeric_limits<double>::lowest();

    for (vertex_id_type v_id = 0; v_id < g.size(); v_id++) {
        if (positions[v_id].x < x_min) {
            x_min = positions[v_id].x;
        }
        if (positions[v_id].x > x_max) {
            x_max = positions[v_id].x;
        }

        if (positions[v_id].y < y_min) {
            y_min = positions[v_id].y;
        }
        if (positions[v_id].y > y_max) {
            y_max = positions[v_id].y;
        }
    }

    double cur_width = x_max - x_min;
    double cur_height = y_max - y_min;

    // Calcul et application de l'echelle
    // 0.9: on se laisse un peu de marge
    double x_scale = 0.9 * width / cur_width;
    double y_scale = 0.9 * height / cur_height;
    double scale = x_scale < y_scale ? x_scale : y_scale;

    double x_center = x_max + x_min;
    double y_center = y_max + y_min;
    double x_offset = x_center / 2 * scale;
    double y_offset = y_center / 2 * scale;

    for (vertex_id_type v_id = 0; v_id < g.size(); v_id++) {
        positions[v_id].x = positions[v_id].x * scale - x_offset;
        positions[v_id].y = positions[v_id].y * scale - y_offset;
    }
}
