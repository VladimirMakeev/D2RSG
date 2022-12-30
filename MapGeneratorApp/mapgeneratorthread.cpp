#include "mapgeneratorthread.h"
#include "mapgenerator.h"
#include "map.h"
#include <stdexcept>

void MapGeneratorThread::run()
{
    try {
        auto scenarioMap = generator->generate();
        emit mapGenerated(scenarioMap.release(), "");
    } catch (const std::exception& e) {
        auto error = QString{"Exception during map generation: "} + e.what();
        emit mapGenerated(nullptr, error);
    }
}

MapGeneratorThread::MapGeneratorThread(rsg::MapGenerator *generator, QObject *parent)
    : QThread(parent)
    , generator{generator}
{
}
