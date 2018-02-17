#include <cairo.h>
#include <cmath>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <streambuf>
#include <string>
#include <vector>

#include "nodesoup.hpp"

#define WIDTH 1024
#define HEIGHT 760

using namespace nodesoup;
using std::string;
using std::vector;

string read_string_from_file(string filename) {
    std::ifstream ifs(filename);
    return string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
}

adj_list_type read_from_dot(string dot) {
    // retourne name sans le 1er caractere
    // ex: v20 -> 20
    auto name_to_vertex_id = [](string name) -> vertex_id_type {
        assert(name[0] == 'v');
        std::istringstream iss(name.substr(1));
        vertex_id_type v_id;
        iss >> v_id;
        return v_id;
    };

    adj_list_type g;
    string line;
    std::istringstream iss(dot);
    while (std::getline(iss, line)) {
        if (line[0] != 'v') {
            continue;
        }

        std::istringstream iss(line);
        string name, edge_sign, adj_name;
        iss >> name >> edge_sign >> adj_name;

        vertex_id_type v_id = name_to_vertex_id(name);
        // add vertex (and preceding vertices) if new
        if (v_id >= g.size()) {
            g.resize(v_id + 1);
            assert(v_id < g.size());
        }

        assert(edge_sign == "--" || edge_sign.size() == 0);
        if (edge_sign != "--") {
            continue;
        }

        vertex_id_type adj_id = name_to_vertex_id(adj_name);

        // add adjacent vertex (and preceding vertices) if new
        if (adj_id >= g.size()) {
            g.resize(adj_id + 1);
            assert(adj_id < g.size());
        }

        // add edge if new
        auto it = find(g[v_id].begin(), g[v_id].end(), adj_id);
        if (it == g[v_id].end()) {
            g[v_id].push_back(adj_id);
            g[adj_id].push_back(v_id);
        }
    }

    return g;
}

void write_to_png(adj_list_type& g, vector<Point2D>& positions, vector<double>& diameters, string filename) {
    // shift origin to 0, 0
    for (vertex_id_type v_id = 0; v_id < g.size(); v_id++) {
        positions[v_id].x += WIDTH / 2.0;
        positions[v_id].y += HEIGHT / 2.0;
    }

    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
    cairo_t* cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 255.0, 255.0, 255.0);
    cairo_paint(cr);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);

    cairo_set_line_width(cr, 1.0);
    for (vertex_id_type v_id = 0; v_id < g.size(); v_id++) {
        Point2D v_pos = positions[v_id];
        cairo_arc(cr, v_pos.x, v_pos.y, diameters[v_id], 0.0, 2.0 * M_PI);
        cairo_fill(cr);

        for (auto adj_id : g[v_id]) {
            // ne pas afficher 2 fois le meme edge
            if (adj_id < v_id) {
                continue;
            }

            Point2D adj_pos = positions[adj_id];
            cairo_move_to(cr, v_pos.x, v_pos.y);
            cairo_line_to(cr, adj_pos.x, adj_pos.y);
            cairo_stroke(cr);
        }
    }

    cairo_surface_write_to_png(surface, filename.c_str());

    cairo_restore(cr);
    cairo_show_page(cr);
}

void layout(string filename, string method) {
    string dot = read_string_from_file(filename);
    adj_list_type g = read_from_dot(dot);

    // TODO
    // Diametres en fonction du degre
    vector<double> diameters;
    diameters.reserve(g.size());
    for (vertex_id_type v_id = 0; v_id < g.size(); v_id++) {
        //positions.push_back(Point2D { random_number(-WIDTH / 2, WIDTH / 2), random_number(- HEIGHT / 2, HEIGHT / 2) });
        diameters.push_back(2.0 + (g[v_id].size() / g.size()));
    }

    // Disposition initiale en cercle
    vector<Point2D> positions(g.size());
    //circle(g, positions);

    if (method == "fr") {
        // Fruchterman Reingold en n iterations
        // TODO verifier combien d'iterations sont utiles
        /*auto cb = [] (vector<Point2D> positions, unsigned int iter) {
            char str[100] = "";
            sprintf(str, "out_%03d.png", j);
            output_graph(g, positions, diameters, str);
        }*/

        positions = fruchterman_reingold(g, WIDTH, HEIGHT);
        write_to_png(g, positions, diameters, "out.png");
    } else {
        positions = kamada_kawai(g, WIDTH, HEIGHT);
        write_to_png(g, positions, diameters, "out.png");
    }
}

void print_usage_and_exit(string binname) {
    std::cerr << "usage: " << binname << " [--gen vertices_count | --layout graph.dot --method (fr|kk)" << std::endl;
    exit(1);
}

int main(int argc, char* argv[]) {
    if (argc < 3 || (string(argv[1]) != "--gen" && string(argv[1]) != "--layout")) {
        print_usage_and_exit(argv[0]);
    }

    if (string(argv[1]) == "--gen") {
        if (argc != 3) {
            print_usage_and_exit(argv[0]);
        }

        unsigned int vertices_count = atoi(argv[2]);
        //gen(vertices_count);
    } else {
        if (argc != 5 || (string(argv[3]) != "--method")) {
            print_usage_and_exit(argv[0]);
        }

        string filename = argv[2];
        if (string(argv[4]) != "fr" && string(argv[4]) != "kk") {
            print_usage_and_exit(argv[0]);
        }

        layout(filename, string(argv[4]));
    }
}
