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
    double  f, g, h; //f-, g- and h-values of the search node
    Node    *parent; //backpointer to the predecessor node (e.g. the node which g-value was used to set the g-velue of the current node)
};

#endif
