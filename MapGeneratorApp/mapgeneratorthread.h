#ifndef MAPGENERATORWORKER_H
#define MAPGENERATORWORKER_H

#include <QThread>
#include <string>

namespace rsg {
class MapGenerator;
class Map;
}

class MapGeneratorThread : public QThread
{
    Q_OBJECT

    void run() override;

public:
    explicit MapGeneratorThread(rsg::MapGenerator* generator, QObject *parent = nullptr);

signals:
    void mapGenerated(rsg::Map* scenarioMap, const QString& error);

private:
    rsg::MapGenerator* generator;
};

#endif // MAPGENERATORWORKER_H
