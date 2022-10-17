#ifndef MAPGENERATORAPP_H
#define MAPGENERATORAPP_H

#include "maptemplate.h"
#include "mapgenerator.h"
#include <filesystem>
#include <memory>
#include <QWidget>
#include <QTimer>

namespace Ui {
class MapGeneratorApp;
}

class MapGeneratorApp : public QWidget
{
    Q_OBJECT

public:
    explicit MapGeneratorApp(QWidget *parent = nullptr);
    ~MapGeneratorApp();

public slots:
    void onScenarioMapGenerated(Map* scenarioMap, const QString& error);
    void seedPlaceholderUpdate();


private slots:
    void on_gameFolderButton_clicked();

    void on_scenarioTemplateButton_clicked();

    void on_generateButton_clicked();

    void on_saveScenarioButtom_clicked();

    void on_scenarioTemplateButtonReload_clicked();

    void on_roadsSpinBox_valueChanged(int roadsPercentage);

    void on_forestSpinBox_valueChanged(int forestPercentage);

    void on_goldSpinBox_valueChanged(int gold);

signals:
    void startGeneration();

private:
    bool readGameInfo(const std::filesystem::path& gameFolder);
    void readTemplateAndUpdateUi(const std::filesystem::path& templatePath);

    // Disable everything, except game folder button
    void disableButtons(bool forGeneration = false);
    void disableRadioButtons(bool forGeneration = false);
    // Enable buttons after generation completes
    void enableButtons();
    // Enable radio buttons according to scenario sizes allowed by template
    void updateRadioButtons();
    void rememberRadioButtonStates();
    void restoreRadioButtonStates();
    // Returns scenario size based on radio buttons state
    int getSelectedScenarioSize();
    std::time_t getScenarioSeed();

    void updatePreviewImages();

    Ui::MapGeneratorApp *ui;

    QTimer seedPlaceholderTimer;
    int seedPlaceholderUpdateTime{1000};

    using MapTemplatePtr = std::unique_ptr<MapTemplate>;
    MapTemplatePtr mapTemplate;
    MapGenOptions options;

    using MapGeneratorPtr = std::unique_ptr<MapGenerator>;
    MapGeneratorPtr generator;

    MapPtr scenario;
    std::filesystem::path templateFilePath;
    bool radioButtons[5];
};

#endif // MAPGENERATORAPP_H
