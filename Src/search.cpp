#include "search.h"
#include <list>

Search::Search() {
    //set defaults here
}

Search::~Search() {}

int abs_int(int i) {
    if (i < 0) {
        return -1 * i;
    }
    return i;
}

double return_H (std::pair<int, int> start, std::pair<int, int> end) {
    return sqrt(pow(start.first - end.first, 2) + pow(start.second - end.second, 2));
}

bool allow_move_to_i_j(int s_i, int s_j, int i, int j, const Map &map, const EnvironmentOptions &options) {
    if (map.getValue(s_i + i, s_j + j) != 0) {
        return false;
    }
    if (i == 0 && j == 0) {
        return false;
    }
    if (abs_int(i) + abs_int(j) == 2) {
        if (options.allowdiagonal == true) {
            int count_near = 0;
            if (map.getValue(s_i, s_j + j) == 1) {
                count_near += 1;
            }
            if (map.getValue(s_i + i, s_j) == 1) {
                count_near += 1;
            }
            if (count_near == 0) {
                return true;
            }
            if (count_near == 1) {
                if (options.cutcorners == true) {
                    return true;
                } else {
                    return false;
                }
            }
            if (count_near == 2) {
                if (options.allowsqueeze == true) {
                    return true;
                } else {
                    return false;
                }
            }
        } else {
            return false;
        }
    }
    return true;
}

SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {
    std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();
    Logger->saveLog();
    bool x = options.allowsqueeze;
    x = false;
    std::vector<Node> all_nodes = {};
    std::vector<Node*> nodes_path = {};
    all_nodes.reserve(size_t(map.getMapFinish().first * map.getMapFinish().second * 2));
    std::map<std::pair<int, int>, Node*> open_nodes = {};
    std::map<std::pair<int, int>, Node*> closed_nodes = {};
    Node start;
    start.cord = map.getMapStart();
    start.g = 0;
    start.h = return_H(map.getMapStart(), map.getMapFinish());
    start.f = start.g + start.h;
    start.parent = nullptr;
    all_nodes.push_back(start);
    open_nodes[start.cord] = &all_nodes[all_nodes.size() - 1];
    while (!open_nodes.empty()) {
        std::pair<std::pair<int, int>, Node*> min_node = *(open_nodes.begin());
        for (auto now_node : open_nodes) {
            if ((*now_node.second).f < (*min_node.second).f) {
                min_node = now_node;
            }
        }
        open_nodes.erase(min_node.first);
        closed_nodes[min_node.first] = min_node.second;
        if (min_node.first == map.getMapFinish()) {
            sresult.pathfound = true;
            sresult.nodescreated = open_nodes.size() + closed_nodes.size();
            sresult.pathlength = (*min_node.second).g;
            Node* cur_node = min_node.second;
            std::list<Node> lresult;
            lresult.push_front(*cur_node);
            while ((*cur_node).parent != nullptr) {
                nodes_path.push_back(cur_node);
                sresult.numberofsteps += 1;
                cur_node = (*cur_node).parent;
                lresult.push_front(*cur_node);
            }
            std::list<Node> hresult = {};
            int i_was = 0, j_was = 0, i_move = 0, j_move = 0;
            bool first = true, second = true;
            Node before;
            for (Node now_node : lresult) {
                if (first == true) {
                    first = false;
                    hresult = {now_node};
                } else {
                    if (second == true) {
                        i_move = now_node.get_i() - i_was;
                        j_move = now_node.get_j() - j_was;
                        second = false;
                    } else {
                        if (!(now_node.get_i() - i_was == i_move && now_node.get_j() - j_was == j_move)) {
                            hresult.push_back(before);
                            i_move = now_node.get_i() - i_was;
                            j_move = now_node.get_j() - j_was;
                        }
                    }
                }
                before = now_node;
                i_was = now_node.get_i();
                j_was = now_node.get_j();
            }
            hresult.push_back(before);
            sresult.lppath = lresult;
            sresult.hppath = hresult;
            std::chrono::time_point<std::chrono::system_clock> stop_time = std::chrono::system_clock::now();
            std::chrono::duration<double> time_of_work = stop_time - start_time;
            sresult.time = time_of_work.count();
            return sresult;
        } else {
            int min_node_i = min_node.first.first;
            int min_node_j = min_node.first.second;
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if (allow_move_to_i_j(min_node_i, min_node_j, i, j, map, options) == true) {
                        Node new_top;
                        new_top.cord = std::pair<int, int> (min_node_i + i, min_node_j + j);
                        if (abs(i) + abs(j) == 2) {
                            new_top.g = (*min_node.second).g + CN_SQRT_TWO;
                        } else {
                            new_top.g = (*min_node.second).g + 1;
                        }
                        if (closed_nodes.count(new_top.cord) == 0) {
                            if (open_nodes.count(new_top.cord) == 0) {
                                new_top.h = return_H(min_node.first, map.getMapFinish());
                                new_top.f = new_top.g + new_top.h;
                                new_top.parent = min_node.second;
                                all_nodes.push_back(new_top);
                                open_nodes[new_top.cord] = &all_nodes[all_nodes.size() - 1];
                            } else {
                                if (new_top.g <= (*open_nodes[new_top.cord]).g) {
                                    (*open_nodes[new_top.cord]).g = new_top.g;
                                    (*open_nodes[new_top.cord]).h = return_H(min_node.first, map.getMapFinish());
                                    (*open_nodes[new_top.cord]).f = new_top.g + new_top.h;
                                    (*open_nodes[new_top.cord]).parent = min_node.second;
                                }
                            }
                        }
                    }
                }
            }
            /* End of search */
        }
    }
    /*sresult.pathfound = ;
    sresult.nodescreated = ;
    sresult.numberofsteps = ;
    sresult.time = ;
    sresult.hppath = &hppath; //Here is a constant pointer
    sresult.lppath = &lppath;*/
    sresult.pathfound = false;
    return sresult;
}

/*void Search::makePrimaryPath(Node curNode)
{
    //need to implement
}*/

/*void Search::makeSecondaryPath()
{
    //need to implement
}*/
