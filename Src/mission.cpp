#include "mission.h"

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
    options.searchtype = config.SearchParams[CN_SP_ST];
    options.allowdiagonal = config.SearchParams[CN_SP_AD];
    options.cutcorners = config.SearchParams[CN_SP_CC];
    options.allowsqueeze = config.SearchParams[CN_SP_AS];
    if (config.N > 4) {
        options.metrictype = config.SearchParams[CN_SP_MT];
        options.heuristicweight = config.SearchParams[CN_SP_HW];
        options.breakingties = config.SearchParams[CN_SP_BT];
    } else {
        options.metrictype = CN_SP_MT_EUCL;
        options.heuristicweight = 1;
        options.breakingties = true;
    }
}

void Mission::createSearch() {
    //might be helpful in case numerous algorithms are added
}

void Mission::startSearch() {
    if (config.LogParams[CN_LP_LEVEL] == CN_LP_LEVEL_FULL_WORD) {
        sr = search.startSearch(map, options, 2);
    } else {
        if (config.LogParams[CN_LP_LEVEL] == CN_LP_LEVEL_MEDIUM_WORD) {
            sr = search.startSearch(map, options, 1);
        } else {
            sr = search.startSearch(map, options, 0);
        }
    }
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
        logger->writeToLogOpenClose(sr.open_close_info, sr.Node_info, sr.map_size, sr.hw, sr.first_Node);
    } else {
        logger->writeToLogNotFound();
    }
    logger->saveLog();
}

void Mission::printPathToPDF() {
    if (sr.pathfound) {
        cairo_surface_t *surface; // Определяем поверхность для рисования
        cairo_t *cr;              // Определяем источник
        int x = map.getMapWidth();
        int y = map.getMapHeight();
        surface = cairo_pdf_surface_create("../../Examples/grid.pdf", x, y); // Создаем поверхность для рисования в виде файла PDF

        for (int i = 0; i < y; ++i) {
            cr = cairo_create(surface);
            cairo_set_source_rgb(cr, 166.0/255, 166.0/255, 166.0/255);
            cairo_set_line_width(cr, 0.0001);
            cairo_move_to(cr, 0, i);
            cairo_line_to(cr, x, i);
            cairo_stroke(cr);
            cairo_destroy(cr);
        }
        for (int i = 0; i < x; ++i) {
            cr = cairo_create(surface);
            cairo_set_source_rgb(cr, 166.0/255, 166.0/255, 166.0/255);
            cairo_set_line_width(cr, 0.0001);
            cairo_move_to(cr, i, 0);
            cairo_line_to(cr, i, y);
            cairo_stroke(cr);
            cairo_destroy(cr);
        }

        for (int x1 = 0; x1 < map.getMapWidth(); ++x1) {
            for (int y1 = 0; y1 < map.getMapHeight(); ++y1) {
                if (map.getValue(y1, x1) == 1) {
                    cr = cairo_create(surface);
                    cairo_set_source_rgb(cr, 0, 0, 0);
                    cairo_set_line_width(cr, 0.0001);
                    cairo_move_to(cr, x1, y1);
                    cairo_line_to(cr, x1, y1+1);
                    cairo_line_to(cr, x1+1, y1+1);
                    cairo_line_to(cr, x1+1, y1);
                    cairo_close_path(cr);
                    cairo_fill(cr);
                    cairo_destroy(cr);
                }
            }
        }
        std::list<Node>::const_iterator iter = sr.hppath.begin();
        std::list<Node>::const_iterator it = sr.hppath.begin();
        while (iter != --sr.hppath.end()) {
            int start_x = it->get_j();
            int start_y = it->get_i();
            ++iter;
            int finish_x = iter->get_j();
            int finish_y = iter->get_i();
            ++it;

            cr = cairo_create(surface);
            cairo_set_source_rgb(cr, 1, 0, 0);
            cairo_set_line_width(cr, 0.1);
            cairo_move_to(cr, start_x + 0.5, start_y + 0.5);
            cairo_line_to(cr, finish_x + 0.5, finish_y + 0.5);
            cairo_stroke(cr);
            cairo_destroy(cr);

            if (start_x != map.getMapStart().second || start_y != map.getMapStart().first) {
                cr = cairo_create(surface);
                cairo_set_source_rgb(cr, 112.0/255, 128.0/255, 144.0/255);
                cairo_set_line_width(cr, 0.001);
                cairo_arc(cr, start_x + 0.5, start_y + 0.5, 0.25, 0, 2*CN_PI_CONSTANT);
                cairo_fill(cr);
                cairo_destroy(cr);
            }
        }

        cr = cairo_create(surface);
        cairo_set_source_rgb(cr, 0, 0, 1);
        cairo_set_line_width(cr, 0.001);
        cairo_arc(cr, map.getMapStart().second + 0.5, map.getMapStart().first + 0.5, 0.25, 0, 2*CN_PI_CONSTANT);
        cairo_fill(cr);
        cairo_destroy(cr);

        cr = cairo_create(surface);
        cairo_set_source_rgb(cr, 0, 0, 1);
        cairo_set_line_width(cr, 0.001);
        cairo_arc(cr, map.getMapFinish().second + 0.5, map.getMapFinish().first + 0.5, 0.25, 0, 2*CN_PI_CONSTANT);
        cairo_fill(cr);
        cairo_destroy(cr);

        cairo_surface_destroy(surface);
    }
}



SearchResult Mission::getSearchResult() {
    return sr;
}

