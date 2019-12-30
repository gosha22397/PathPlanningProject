#include "mission.h"
#include <iostream>

Mission::Mission() {
    logger = nullptr;
    fileName = nullptr;
}

Mission::Mission(const char *FileName) {
    fileName = FileName;
    logger = nullptr;
}

Mission::~Mission() {
    if (logger)
        delete logger;
}

bool Mission::getMap() {
    return map.getMap(fileName);
}

bool Mission::getConfig() {
    return config.getConfig(fileName);
}

bool Mission::createLog() {
    if (logger != nullptr) delete logger;
    logger = new XmlLogger(config.LogParams[CN_LP_LEVEL]);
    return logger->getLog(fileName, config.LogParams);
}

void Mission::createEnvironmentOptions() {
    options.cutcorners = config.SearchParams[CN_SP_CC];
    options.allowsqueeze = config.SearchParams[CN_SP_AS];
    options.allowdiagonal = config.SearchParams[CN_SP_AD];
    options.metrictype = config.SearchParams[CN_SP_MT];

}

void Mission::createSearch() {
    //might be helpful in case numerous algorithms are added
}

void Mission::startSearch() {
    sr = search.startSearch(logger, map, options);
}

void Mission::printSearchResultsToConsole() {
    std::cout << "Path ";
    if (!sr.pathfound) {
        std::cout << "NOT ";
    }
    std::cout << "found!\n";
    std::cout << "numberofsteps=" << sr.numberofsteps << '\n';
    std::cout << "nodescreated=" << sr.nodescreated << '\n';
    if (sr.pathfound) {
        std::cout << "pathlength=" << sr.pathlength << '\n';
        std::cout << "pathlength_scaled=" << sr.pathlength * float(map.getCellSize()) << '\n';
    }
    std::cout << "time=" << sr.time << '\n';
}

void Mission::saveSearchResultsToLog() {
    logger->writeToLogSummary(sr.numberofsteps, sr.nodescreated, sr.pathlength, sr.time, map.getCellSize());
    if (sr.pathfound) {
        logger->writeToLogPath(sr.lppath);
        logger->writeToLogHPpath(sr.hppath);
        logger->writeToLogMap(map, sr.lppath);
    } else {
        logger->writeToLogNotFound();
    }
    logger->saveLog();
}

SearchResult Mission::getSearchResult() {
    return sr;
}

