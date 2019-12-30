#include "search.h"
#include <list>
#include <set>

Search::Search() {
    //set defaults here
}

Search::~Search() {}

int min_int(int i, int j) {
    if (i > j) {
        return j;
    }
    return i;
}

int max_int(int i, int j) {
    if (i > j) {
        return i;
    }
    return j;
}

double return_H (std::pair<int, int> start, std::pair<int, int> end, const EnvironmentOptions &options) {
    if (options.metrictype == 0) {
        auto i = abs(start.first - end.first);
        auto j = abs(start.second - end.second);
        return (max_int(i, j) - min_int(i, j)) + CN_SQRT_TWO * min_int(i, j);
    }
    if (options.metrictype == 1) {
        return abs(start.first - end.first) + abs(start.second - end.second);
    }
    if (options.metrictype == 2) {
        return sqrt(pow(start.first - end.first, 2) + pow(start.second - end.second, 2));
    }
    if (options.metrictype == 3) {
        return max_int(abs(start.first - end.first), abs(start.second - end.second));
    }
    return 0;
}

bool allow_move_to_i_j(int s_i, int s_j, int i, int j, const Map &map, const EnvironmentOptions &options) {
    if (map.getValue(s_i + i, s_j + j) != 0) {
        return false;
    }
    if (i == 0 && j == 0) {
        return false;
    }
    if (abs(i) + abs(j) == 2) {
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
    std::set<std::pair<int, int>> open_nodes = {};
    std::set<std::pair<int, int>> closed_nodes = {};
    std::map<std::pair<int, int>, std::pair<int, int>> before_point;
    std::map<std::pair<int, int>, Node> Node_info;
    Node start;
    start.cord = map.getMapStart();
    start.g = 0;
    start.h = return_H(map.getMapStart(), map.getMapFinish(), options);
    Node_info[start.cord] = start;
    open_nodes.insert(start.cord);
    unsigned int number_of_steps = 0;
    while (!open_nodes.empty()) {
        number_of_steps += 1;
        std::pair<int, int> min_node_addr = *(open_nodes.begin());
        for (auto now_node_addr : open_nodes) {
            if (Node_info[now_node_addr].get_f() < Node_info[min_node_addr].get_f()) {
                min_node_addr = now_node_addr;
            }
        }
        open_nodes.erase(min_node_addr);
        closed_nodes.insert(min_node_addr);
        if (min_node_addr == map.getMapFinish()) {
            sresult.pathfound = true;
            sresult.numberofsteps = number_of_steps;
            sresult.nodescreated = open_nodes.size() + closed_nodes.size();
            sresult.pathlength = float(Node_info[min_node_addr].g);
            Node cur_node = Node_info[min_node_addr];
            std::list<Node> lresult;
            lresult.push_front(cur_node);
            while (cur_node.cord != map.getMapStart()) {
                cur_node = Node_info[before_point[cur_node.cord]];
                lresult.push_front(cur_node);
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
            int min_node_i = min_node_addr.first;
            int min_node_j = min_node_addr.second;
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if (allow_move_to_i_j(min_node_i, min_node_j, i, j, map, options) == true) {
                        Node new_top;
                        new_top.cord = std::pair<int, int> (min_node_i + i, min_node_j + j);
                        if (abs(i) + abs(j) == 2) {
                            new_top.g = Node_info[min_node_addr].g + CN_SQRT_TWO;
                        } else {
                            new_top.g = Node_info[min_node_addr].g + 1;
                        }
                        new_top.h = return_H(new_top.cord, map.getMapFinish(), options);
                        if (closed_nodes.count(new_top.cord) == 0) {
                            if (open_nodes.count(new_top.cord) == 0) {
                                open_nodes.insert(new_top.cord);
                                Node_info[new_top.cord] = new_top;
                                before_point[new_top.cord] = min_node_addr;
                            } else {
                                if (new_top.g <= Node_info[new_top.cord].g) {
                                    Node_info[new_top.cord] = new_top;
                                    before_point[new_top.cord] = min_node_addr;
                                }
                            }
                        }
                    }
                }
            }
            /* End of search */
        }
    }
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
