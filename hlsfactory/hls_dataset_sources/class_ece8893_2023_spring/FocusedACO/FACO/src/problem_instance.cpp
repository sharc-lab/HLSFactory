/**
 * @author: Rafał Skinderowicz (rafal.skinderowicz@us.edu.pl)
 */
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "problem_instance.h"

// This comes from https://stackoverflow.com/a/217605
// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// This comes from https://stackoverflow.com/a/217605
// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// This comes from https://stackoverflow.com/a/217605
// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

/**
 * Tries to load a Traveling Salesman Problem (or ATSP) instance in TSPLIB
 * format from file at 'path'. Only the instances with 'EDGE_WEIGHT_TYPE:
 * EUC_2D' or 'EXPLICIT' are supported.
 *
 * Throws runtime_error if the file is in unsupported format or if an error was
 * encountered.
 *
 * Returns the loaded problem instance.
 */
ProblemInstance load_tsplib_instance(const char *path) {
    using namespace std;
    enum EdgeWeightFormat { UPPER_DIAG_ROW, LOWER_DIAG_ROW, UPPER_ROW, FUNCTION };

    ifstream in(path);

    if (!in.is_open()) {
        throw runtime_error(string("Cannot open TSP instance file: ") + path);
    }

    string line;

    uint32_t dimension = 0;
    vector<double> distances;
    vector<Vec2d> coords;
    EdgeWeightType edge_weight_type{EUC_2D};
    EdgeWeightFormat edge_weight_format { UPPER_DIAG_ROW };
    bool is_symmetric = true;
    string name = "Unknown";

    cout << "Loading TSP instance from file:" << path << "\n";

    while (getline(in, line)) {
        cout << '\t' << line << endl;
        if (line.find("NAME") == 0) {
            name = line.substr(line.find(':') + 1);
            trim(name);
        } else if (line.find("TYPE") == 0) {
            if (line.find(" TSP") != string::npos) {
                is_symmetric = true;
            } else if (line.find(" ATSP") != string::npos) {
                is_symmetric = false;
            } else {
                throw runtime_error("Unknown problem type");
            }
        } else if (line.find("DIMENSION") != string::npos) {
            istringstream line_in(line.substr(line.find(':') + 1));
            if (!(line_in >> dimension)) {
                throw runtime_error(string("Cannot read instance dimension"));
            }
        } else if (line.find("EDGE_WEIGHT_TYPE") != string::npos) {
            if (line.find(" EUC_2D") != string::npos) {
                edge_weight_type = EUC_2D;
            } else if (line.find(" CEIL_2D") != string::npos) {
                edge_weight_type = CEIL_2D;
            } else if (line.find(" EXPLICIT") != string::npos) {
                edge_weight_type = EXPLICIT;
            } else if (line.find(" GEO") != string::npos) {
                edge_weight_type = GEO;
            } else if (line.find(" ATT") != string::npos) {
                edge_weight_type = ATT;
            } else {
                throw runtime_error(string("Unsupported edge weight type"));
            }
        } else if (line.find("EDGE_WEIGHT_FORMAT") != string::npos) {
            if (line.find(" UPPER_DIAG_ROW") != string::npos) {
                edge_weight_format = UPPER_DIAG_ROW;
            } else if (line.find(" LOWER_DIAG_ROW") != string::npos) {
                edge_weight_format = LOWER_DIAG_ROW;
            } else if (line.find(" UPPER_ROW") != string::npos) {
                edge_weight_format = UPPER_ROW;
            } else if (line.find(" FUNCTION") != string::npos) {
                edge_weight_format = FUNCTION;
            } else {
                throw runtime_error(string("Unsupported edge weight format"));
            }
        } else if (line.find("NODE_COORD_SECTION") != string::npos) {
            while (coords.size() < dimension && getline(in, line)) {
                if (line.find("EOF") != string::npos) {
                    break ;
                }
                istringstream line_in(line);
                uint32_t id;
                Vec2d point {};
                if (line_in >> id >> point.x_ >> point.y_) {
                    coords.push_back(point);
                } else {
                    cerr << "Error while reading coordinates! A pair of floats was expected.";
                    abort();  // We should not continue without checking the input file first
                }
            }
        } else if (line.find("EDGE_WEIGHT_SECTION") != string::npos) {
            assert(dimension > 0);
            if (edge_weight_type != EXPLICIT) {
                throw runtime_error("Expected EXPLICIT edge weight type");
            }

            if (edge_weight_format == UPPER_DIAG_ROW) {
                distances.resize(dimension * dimension);

                uint32_t row = 0;
                uint32_t col = 0;
                while (row < dimension && getline(in, line)) {
                    istringstream line_in(line);
                    double distance;
                    while (line_in >> distance) {
                        distances.at(row * dimension + col) = distance;
                        distances.at(col * dimension + row) = distance;
                        ++col;
                        if (col == dimension) {
                            ++row;
                            col = row;
                        }
                    }
                }
            } else if (edge_weight_format == UPPER_ROW) {
                distances.resize(dimension * dimension);

                uint32_t row = 0;
                uint32_t col = 1;
                while (row < dimension && getline(in, line)) {
                    istringstream line_in(line);
                    double distance;
                    while (line_in >> distance) {
                        distances.at(row * dimension + col) = distance;
                        distances.at(col * dimension + row) = distance;
                        ++col;
                        if (col == dimension) {
                            ++row;
                            col = row + 1;
                        }
                    }
                }
            } else if (edge_weight_format == LOWER_DIAG_ROW) {
                distances.resize(dimension * dimension, 0);

                uint32_t row = 0;
                uint32_t col = 0;
                while (row < dimension && getline(in, line)) {
                    istringstream line_in(line);
                    double distance;
                    while (line_in >> distance) {
                        distances.at(row * dimension + col) = distance;
                        distances.at(col * dimension + row) = distance;
                        ++col;
                        if (col == row + 1) {
                            ++row;
                            col = 0;
                            if (row == dimension) {
                                break ;
                            }
                        }
                    }
                }
            } else {
                distances.reserve(dimension * dimension);
                while (getline(in, line)) {
                    if (line.find("EOF") != string::npos) {
                        break;
                    }
                    istringstream line_in(line);
                    double distance;
                    while (line_in >> distance) {
                        distances.push_back(distance);
                    }
                }
            }
            assert(distances.size() == dimension * dimension);
        }
    }
    in.close();

    assert(dimension > 2);

    return ProblemInstance(dimension,
                           edge_weight_type,
                           coords, distances,
                           is_symmetric, name);
}


void route_to_svg(const ProblemInstance &instance,
                  const std::vector<uint32_t> &route,
                  const std::string &path) {
    using namespace std;

    if (instance.coords_.empty()) {  // No coords., no picture
        return ;
    }

    ofstream out(path);
    if (out.is_open()) {
        auto p = instance.coords_.at(0);
        auto min_x = p.x_;
        auto max_x = min_x;
        auto min_y = p.y_;
        auto max_y = min_y;

        for (auto &c : instance.coords_) {
            min_x = min(min_x, c.x_);
            max_x = max(max_x, c.x_);
            min_y = min(min_y, c.y_);
            max_y = max(max_y, c.y_);
        }

        // We are scaling the image so that the width equals 1000.0,
        // and the height is scaled proportionally (keeping the original
        // ratio).
        auto width  = max_x - min_x;
        auto height = max_y - min_y;

        auto hw_ratio = height / width;

        auto svg_width  = 1000.0;
        auto svg_height = svg_width * hw_ratio;

        out << "<?xml version=\"1.0\" standalone=\"no\"?>\n"
            << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
            << "<svg version=\"1.1\""
            << " viewBox=\"" << 0
            << " " << 0
            << " " << svg_width
            << " " << svg_height
            << "\">\n"
            << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";


        auto prev_id = route.back();
        p = instance.coords_.at(prev_id);

        auto x = p.x_;
        auto y = p.y_;

        x = svg_width - (x - min_x) / width * svg_width;
        y = (y - min_y) / height * svg_height;

        out << R"(<polyline fill="none" stroke="black" stroke-width="0.5" points=")"
            << x << "," << y;

        for (auto id : route) {
            p = instance.coords_.at(id);
            x = p.x_;
            y = p.y_;
            x = svg_width - (x - min_x) / width * svg_width;
            y = (y - min_y) / height * svg_height;
            out << " " << x << "," << y;
        }
        out << "\"/>\n";
        out << "</svg>";

        out.close();
    }
}
