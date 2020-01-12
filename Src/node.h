#ifndef NODE_H
#define NODE_H
#include <utility>

//That's the data structure for storing a single search node.
//You MUST store all the intermediate computations occuring during the search
//incapsulated to Nodes (so NO separate arrays of g-values etc.)

struct Node {
    std::pair<int, int> cord; //grid cell coordinates
    int get_i() const { // i for xmllogger
        return cord.first;
    }
    int get_j() const { // j for xmllogger
        return cord.second;
    }
    double g, h; // g- and h-values of the search node
    double get_f() const { // get f-value of the search node
        return g + /*EnvironmentOptions::get_heuristicweight() */ h;
    }
    double get_g() const { // get g-value of the search node
        return g;
    }
};

#endif
