#include "environmentoptions.h"

EnvironmentOptions::EnvironmentOptions() {
    searchtype = CN_SP_ST_DIJK;
    allowdiagonal = true;
    cutcorners = false;
    allowsqueeze = false;
    k_degree_of_neighborhood = 2;
    metrictype = CN_SP_MT_EUCL;
    heuristicweight = 1;
    breakingties = true;
}

EnvironmentOptions::EnvironmentOptions(int ST, bool AD, bool CC, bool AS, int DK, int MT, int HW, bool BT) {
    searchtype = ST;
    allowdiagonal = AD;
    cutcorners = CC;
    allowsqueeze = AS;
    k_degree_of_neighborhood = DK;
    metrictype = MT;
    heuristicweight = HW;
    breakingties = BT;
}

/*
int EnvironmentOptions::get_heuristicweight() {
    return heuristicweight;
}
*/
