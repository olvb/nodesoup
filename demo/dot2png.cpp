#include <algorithm>
#include <cairo.h>
#include <cassert>
#include <cmath>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include "nodesoup.hpp"

using namespace nodesoup;
using std::cerr;
using std::string;
using std::vector;

/** Ready simple-enough dot files (one edge per line, vertex labels "v0", "v1", etc) */
adj_list_type read_from_dot(string filename) {
    std::ifstream ifs(filename);
    if (!ifs.good()) {
        cerr << "Could not open file \"" << filename << "\"\n";
        exit(EXIT_FAILURE);
    }

    auto name_to_vertex_id = [](string name) -> vertex_id_type {
        assert(name[0] == 'v');
        std::istringstream iss(name.substr(1));
        vertex_id_type v_id;
        iss >> v_id;
        return v_id;
    };

    adj_list_type g;
    string line;
    while (std::getline(ifs, line)) {
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

void write_to_png(adj_list_type& g, vector<Point2D>& positions, vector<double>& diameters, unsigned int width, unsigned int height, string filename) {
    // shift origin to 0, 0
    for (vertex_id_type v_id = 0; v_id < g.size(); v_id++) {
        positions[v_id].x += width / 2.0;
        positions[v_id].y += height / 2.0;
    }

    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
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
            if (adj_id < v_id) {
                continue;
            }

            Point2D adj_pos = positions[adj_id];
            cairo_move_to(cr, v_pos.x, v_pos.y);
            cairo_line_to(cr, adj_pos.x, adj_pos.y);
            cairo_stroke(cr);
        }
    }

    if (cairo_surface_write_to_png(surface, filename.c_str()) != CAIRO_STATUS_SUCCESS) {
        cerr << "Could not write to file \"" << filename << "\"\n";
        exit(EXIT_FAILURE);
    }

    cairo_restore(cr);
    cairo_show_page(cr);
}

enum Method { fr,
    kk };

void dot2png(
    string dot_filename,
    string png_filename,
    Method method,
    unsigned int width,
    unsigned int height,
    double k,
    double energy_threshold,
    int iters_count) {
    adj_list_type g = read_from_dot(dot_filename);

    // Vertices diameters grow with degree
    // TODO consider connectivity?
    double base_diameter = 1.0;
    vector<double> diameters;
    diameters.reserve(g.size());
    for (vertex_id_type v_id = 0; v_id < g.size(); v_id++) {
        double diameter = base_diameter + (double) g[v_id].size();
        diameters.push_back(diameter);
    }

    vector<Point2D> positions;
    if (method == Method::fr) {
        // Fruchterman Reingold en n iterations
        // TODO verifier combien d'iterations sont utiles
        /*auto cb = [] (vector<Point2D> positions, unsigned int iter) {
            char str[100] = "";
            sprintf(str, "out_%03d.png", j);
            output_graph(g, positions, diameters, str);
        }*/
        if (k == -1.0) {
            k = 10.0;
        }
        positions = fruchterman_reingold(g, width, height, iters_count, k);
        write_to_png(g, positions, diameters, width, height, png_filename);
    } else {
        if (k == -1.0) {
            k = 300.0;
        }
        positions = kamada_kawai(g, width, height, k, energy_threshold);
        write_to_png(g, positions, diameters, width, height, png_filename);
    }
}

void print_usage_and_exit(string exec_name) {
    cerr << "Usage: " << exec_name << " [options] <in.dot> <out.png>\n";
    cerr << "Options:\n";
    cerr << "  -m <method>\t\tLayout method to use between Fruchterman Reingold and Kamada Kawai [fr|kk, default: fr]\n";
    cerr << "  -w <width>\t\tCanvas width in pixels [default: 1024]\n";
    cerr << "  -h <height>\t\tCanvas height in pixels [default: 760]\n";
    cerr << "  -k <strength>\t\tStrength factor [default: 10 for fr, 300 for kk]\n";
    cerr << "  -i <iterations>\tNumber of iterations for fr [default: 100]\n";
    cerr << "  -e <epsilon>\t\tEnergy threshold for kk [default: 1e-2]\n";
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    Method method = fr;
    int width = 1024;
    int height = 768;
    double k = -1.0;
    double energy_threshold = 1e-2;
    int iters_count = 100;

    char opt;
    while ((opt = getopt(argc, argv, "m:w:h:k:e:i:")) != -1) {
        switch (opt) {
        case 'm':
            if (string(optarg) == "fr") {
                method = fr;
            } else if (string(optarg) == "kk") {
                method = kk;
            } else {
                cerr << "Invalid method: \"" << optarg << "\"\n";
                print_usage_and_exit(argv[0]);
            }
            break;
        case 'w':
            width = atoi(optarg);
            if (width <= 0) {
                cerr << "Invalid width: \"" << width << "\"\n";
                print_usage_and_exit(argv[0]);
            }
            break;
        case 'h':
            height = atoi(optarg);
            if (height <= 0) {
                cerr << "Invalid height: \"" << height << "\"\n";
                print_usage_and_exit(argv[0]);
            }
            break;
        case 'k':
            k = atof(optarg);
            if (k <= 0.0) {
                cerr << "Invalid k value: \"" << k << "\"\n";
                print_usage_and_exit(argv[0]);
            }
            break;
        case 'i':
            iters_count = atoi(optarg);
            if (iters_count <= 0) {
                cerr << "Invalid iterations: \"" << iters_count << "\"\n";
                print_usage_and_exit(argv[0]);
            }
            break;
        case 'e':
            energy_threshold = atof(optarg);
            if (energy_threshold <= 0) {
                cerr << "Invalid energy threshold: \"" << energy_threshold << "\"\n";
                print_usage_and_exit(argv[0]);
            }
            break;
        default:
            print_usage_and_exit(argv[0]);
        }
    }

    if (argc - optind != 2) {
        print_usage_and_exit(argv[0]);
    }

    char* dot_filename = argv[optind];
    char* png_filename = argv[optind + 1];
    dot2png(dot_filename, png_filename, method, width, height, k, energy_threshold, iters_count);
}
