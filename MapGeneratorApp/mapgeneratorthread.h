#ifndef MAPGENERATORWORKER_H
#define MAPGENERATORWORKER_H

#include <QThread>
#include <string>

class MapGenerator;
class Map;

class MapGeneratorThread : public QThread
{
    Q_OBJECT

    void run() override;

public:
    explicit MapGeneratorThread(MapGenerator* generator, QObject *parent = nullptr);

signals:
    void mapGenerated(Map* scenarioMap, const QString& error);

private:
    MapGenerator* generator;
};

#endif // MAPGENERATORWORKER_H
