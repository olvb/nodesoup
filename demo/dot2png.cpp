#include "nodesoup.hpp"
#include <algorithm>
#include <cairo.h>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <unordered_map>
#include <vector>

using namespace nodesoup;
using namespace std;

/** Read not-too-complicated dot files */
adj_list_t read_from_dot(string filename) {
    std::ifstream ifs(filename);
    if (!ifs.good()) {
        cerr << "Could not open file \"" << filename << "\"\n";
        exit(EXIT_FAILURE);
    }

    adj_list_t g;
    unordered_map<string, vertex_id_t> names;

    auto name_to_vertex_id = [&g, &names](string name) -> vertex_id_t {
        if (name[name.size() - 1] == ';') {
            name.erase(name.end() - 1, name.end());
        }

        vertex_id_t v_id;
        auto it = names.find(name);
        if (it != names.end()) {
            return (*it).second;
        }

        v_id = g.size();
        names.insert({ name, v_id });
        g.resize(v_id + 1);
        return v_id;
    };

    string line;
    // skip first line
    std::getline(ifs, line);

    while (std::getline(ifs, line)) {
        if (line[0] == '}') {
            break;
        }

        std::istringstream iss(line);
        string name, edge_sign, adj_name;
        iss >> name >> edge_sign >> adj_name;

        // add vertex if new
        vertex_id_t v_id = name_to_vertex_id(name);

        assert(edge_sign == "--" || edge_sign.size() == 0);
        if (edge_sign != "--") {
            continue;
        }

        // add adjacent vertex if new
        vertex_id_t adj_id = name_to_vertex_id(adj_name);

        // add edge if new
        if (find(g[v_id].begin(), g[v_id].end(), adj_id) == g[v_id].end()) {
            g[v_id].push_back(adj_id);
            g[adj_id].push_back(v_id);
        }
    }
    return g;
}

void write_to_png(adj_list_t& g, vector<Point2D>& positions, vector<double>& radiuses, unsigned int width, unsigned int height, string filename) {
    // shift origin to 0, 0
    for (vertex_id_t v_id = 0; v_id < g.size(); v_id++) {
        positions[v_id].x += width / 2.0;
        positions[v_id].y += height / 2.0;
    }

    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t* cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 255.0, 255.0, 255.0);
    cairo_paint(cr);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);

    cairo_set_line_width(cr, 1.0);
    for (vertex_id_t v_id = 0; v_id < g.size(); v_id++) {
        Point2D v_pos = positions[v_id];
        cairo_arc(cr, v_pos.x, v_pos.y, radiuses[v_id], 0.0, 2.0 * M_PI);
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

void dot_to_png(
    string dot_filename,
    string png_filename,
    Method method,
    unsigned int width,
    unsigned int height,
    double k,
    double energy_threshold,
    int iters_count,
    bool animated) {
    adj_list_t g = read_from_dot(dot_filename);

    vector<Point2D> positions;
    vector<double> radiuses = size_radiuses(g);

    chrono::time_point<chrono::system_clock> start, end;
    cout << "Laying out graph...\n";
    // Fruchterman-Reingold
    if (method == Method::fr) {
        iter_callback_t cback = nullptr;
        char* frame_filename = nullptr;
        string frame_filename_format;

        // support for outputing (almost) all frames
        if (animated) {
            size_t index = png_filename.rfind(".png");
            if (index == string::npos) {
                frame_filename_format = png_filename + "_%03d";
            } else {
                frame_filename_format = png_filename.substr(0, index) + "_%03d.png";
            }
            frame_filename = new char[frame_filename_format.size()];

            cback = [&g, &radiuses, width, height, iters_count, frame_filename, &frame_filename_format](vector<Point2D> positions, unsigned int iter) {
                if (iter % 2 != 0 && iter != 0 && iter != iters_count - 1) {
                    return;
                }
                sprintf(frame_filename, frame_filename_format.c_str(), iter);
                write_to_png(g, positions, radiuses, width, height, frame_filename);
            };
        }

        if (k == -1.0) {
            k = 15.0;
        }

        start = chrono::system_clock::now();
        positions = fruchterman_reingold(g, width, height, iters_count, k, cback);
        end = chrono::system_clock::now();

        if (animated) {
            delete[] frame_filename;
        } else {
            write_to_png(g, positions, radiuses, width, height, png_filename);
        }
    }
    // Kamada-Kawai
    else {
        if (k == -1.0) {
            k = 300.0;
        }

        start = chrono::system_clock::now();
        positions = kamada_kawai(g, width, height, k, energy_threshold);
        end = chrono::system_clock::now();

        write_to_png(g, positions, radiuses, width, height, png_filename);
    }

    unsigned int ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "Layout computed in " << ms << "ms\n";
}

void usage(string exec_name) {
    cerr << "Usage: " << exec_name << " [options] <in.dot> <out.png>\n";
    cerr << "Options:\n";
    cerr << "  -m <method>\t\tLayout method to use between Fruchterman Reingold and Kamada Kawai [fr|kk, default: fr]\n";
    cerr << "  -w <width>\t\tCanvas width in pixels [default: 1024]\n";
    cerr << "  -h <height>\t\tCanvas height in pixels [default: 760]\n";
    cerr << "  -k <strength>\t\tStrength factor [default: 10 for fr, 300 for kk]\n";
    cerr << "  -i <nb_iterations>\tNumber of iterations for fr [default: 100]\n";
    cerr << "  -e <epsilon>\t\tEnergy threshold for kk [default: 1e-2]\n";
    cerr << "  -a\t\t\tOutput all intermediary frames for fr [default: false]\n";
}

int main(int argc, char* argv[]) {
    Method method = fr;
    int width = 1024;
    int height = 768;
    double k = -1.0;
    double energy_threshold = 1e-2;
    int iters_count = 300;
    bool animated = false;

    char opt;
    while ((opt = getopt(argc, argv, "m:w:h:k:e:i:a")) != -1) {
        switch (opt) {
        case 'm':
            if (string(optarg) == "fr") {
                method = fr;
            } else if (string(optarg) == "kk") {
                method = kk;
            } else {
                cerr << "Invalid method: \"" << optarg << "\"\n";
                usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        case 'w':
            width = atoi(optarg);
            if (width <= 0) {
                cerr << "Invalid width: \"" << width << "\"\n";
                usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        case 'h':
            height = atoi(optarg);
            if (height <= 0) {
                cerr << "Invalid height: \"" << height << "\"\n";
                usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        case 'k':
            k = atof(optarg);
            if (k <= 0.0) {
                cerr << "Invalid k value: \"" << k << "\"\n";
                usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        case 'i':
            iters_count = atoi(optarg);
            if (iters_count <= 0) {
                cerr << "Invalid iterations: \"" << iters_count << "\"\n";
                usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        case 'e':
            energy_threshold = atof(optarg);
            if (energy_threshold <= 0) {
                cerr << "Invalid energy threshold: \"" << energy_threshold << "\"\n";
                usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        case 'a':
            animated = true;
            break;
        default:
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (argc - optind != 2) {
        cerr << "Missing positional arguments\n";
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    char* dot_filename = argv[optind];
    char* png_filename = argv[optind + 1];
    dot_to_png(dot_filename, png_filename, method, width, height, k, energy_threshold, iters_count, animated);
}
