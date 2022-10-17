#include "mapgeneratorapp.h"
#include "ui_mapgeneratorapp.h"
#include "gameinfo.h"
#include "maptemplatereader.h"
#include "mapgenerator.h"
#include "mapgeneratorthread.h"
#include "image.h"
#include "version.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>

MapGeneratorApp::MapGeneratorApp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapGeneratorApp)
{
    ui->setupUi(this);

    ui->aboutLabel->setText(tr("v %1, mak").arg(VER_PRODUCTVERSION_STR));

    connect(&seedPlaceholderTimer, &QTimer::timeout, this, &MapGeneratorApp::seedPlaceholderUpdate);

    disableButtons();
}

MapGeneratorApp::~MapGeneratorApp()
{
    delete ui;
}

void MapGeneratorApp::onScenarioMapGenerated(Map *scenarioMap, const QString &error)
{
    // Enable buttons
    enableButtons();

    if (!error.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), error);
        return;
    }

    scenario.reset(scenarioMap);
    // Allow to save generated scenario
    ui->saveScenarioButtom->setEnabled(true);
    // Update zone and contents images
    updatePreviewImages();
}

void MapGeneratorApp::seedPlaceholderUpdate()
{
    ui->seedEdit->setPlaceholderText(QString("%1").arg(std::time(nullptr)));

}

void MapGeneratorApp::on_gameFolderButton_clicked()
{
    const QString dir = QFileDialog::getExistingDirectory(this,
                                                          tr("Choose game folder"),
                                                          "",
                                                          QFileDialog::ShowDirsOnly
                                                          | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        // Canceled by user
        return;
    }

    const std::filesystem::path gameFolder{dir.toStdString()};

    if (!readGameInfo(gameFolder)) {
        disableButtons();

        QMessageBox::critical(this, tr("Error"), tr("Could not read game info"));
        return;
    }

    // Allow user to select templates
    ui->scenarioTemplateButton->setEnabled(true);
    // Show chosen game folder in ui
    ui->gameFolderEdit->setText(dir);
}

bool MapGeneratorApp::readGameInfo(const std::filesystem::path &gameFolder)
{
    if (!std::filesystem::exists(gameFolder)) {
        return false;
    }

    return ::readGameInfo(gameFolder);
}

void MapGeneratorApp::readTemplateAndUpdateUi(const std::filesystem::path& templatePath)
{
    try {
        MapTemplate* tmplt = readMapTemplate(templatePath);

        mapTemplate.reset(tmplt);
        templateFilePath = templatePath;
    }
    catch (const std::runtime_error& e) {
        QMessageBox::critical(this, tr("Error"), tr("Could not read template file\n%1").arg(e.what()));
        return;
    }

    // Show chosen template in ui
    ui->scenarioTemplateEdit->setText(QString(templatePath.stem().string().c_str()));
    // Allow user to select generator seed
    ui->seedEdit->setEnabled(true);
    // Update seedEdit placeholder text with seed of current time
    seedPlaceholderTimer.start(seedPlaceholderUpdateTime);
    // Allow user to refresh template
    ui->scenarioTemplateButtonReload->setEnabled(true);
    // Allow uset to change roads, forests and starting gold
    ui->goldSpinBox->setValue(mapTemplate->startingGold);
    ui->goldSpinBox->setEnabled(true);

    ui->roadsSpinBox->setValue(mapTemplate->roads);
    ui->roadsSpinBox->setEnabled(true);

    ui->forestSpinBox->setValue(mapTemplate->forest);
    ui->forestSpinBox->setEnabled(true);

    // Allow user to select scenario size
    updateRadioButtons();
    // Allow user to generate scenario
    ui->generateButton->setEnabled(true);
}

void MapGeneratorApp::disableButtons(bool forGeneration)
{
    if (forGeneration) {
        ui->gameFolderButton->setEnabled(false);
    }

    ui->scenarioTemplateButton->setEnabled(false);
    ui->scenarioTemplateButtonReload->setEnabled(false);
    ui->saveScenarioButtom->setEnabled(false);

    if (!forGeneration) {
        ui->seedEdit->clear();
        ui->goldSpinBox->clear();
        ui->roadsSpinBox->clear();
        ui->forestSpinBox->clear();
    }

    seedPlaceholderTimer.stop();
    ui->seedEdit->setEnabled(false);

    ui->goldSpinBox->setEnabled(false);

    ui->roadsSpinBox->setEnabled(false);
    ui->forestSpinBox->setEnabled(false);

    ui->generateButton->setEnabled(false);
    disableRadioButtons(forGeneration);
}

void MapGeneratorApp::disableRadioButtons(bool forGeneration)
{
    QRadioButton *buttons[5] = {
        ui->size48Button,
        ui->size72Button,
        ui->size96Button,
        ui->size120Button,
        ui->size144Button
    };

    for (auto& button : buttons) {
        button->setEnabled(false);

        if (!forGeneration) {
            button->setChecked(false);
        }
    }
}

void MapGeneratorApp::enableButtons()
{
    ui->gameFolderButton->setEnabled(true);
    ui->scenarioTemplateButton->setEnabled(true);
    ui->scenarioTemplateButtonReload->setEnabled(true);

    ui->goldSpinBox->setEnabled(true);

    ui->roadsSpinBox->setEnabled(true);
    ui->forestSpinBox->setEnabled(true);

    ui->seedEdit->setEnabled(true);
    seedPlaceholderTimer.start(seedPlaceholderUpdateTime);

    ui->generateButton->setEnabled(true);
    restoreRadioButtonStates();
}

void MapGeneratorApp::updateRadioButtons()
{
    disableRadioButtons();

    QRadioButton *buttons[5] = {
        ui->size48Button,
        ui->size72Button,
        ui->size96Button,
        ui->size120Button,
        ui->size144Button
    };

    const auto startIndex = mapTemplate->sizeMin / 24 - 2;
    const auto endIndex = mapTemplate->sizeMax / 24 - 2;

    // Enable buttons that correspond to allowed template sizes
    for (int i = startIndex; i <= endIndex && i < 5; ++i) {
        buttons[i]->setEnabled(true);
    }

    // Choose minimum allowed size by default
    buttons[startIndex]->setChecked(true);
}

void MapGeneratorApp::rememberRadioButtonStates()
{
    QRadioButton *buttons[5] = {
        ui->size48Button,
        ui->size72Button,
        ui->size96Button,
        ui->size120Button,
        ui->size144Button
    };

    for (int i = 0; i < 5; ++i) {
        radioButtons[i] = buttons[i]->isEnabled();
    }
}

void MapGeneratorApp::restoreRadioButtonStates()
{
    QRadioButton *buttons[5] = {
        ui->size48Button,
        ui->size72Button,
        ui->size96Button,
        ui->size120Button,
        ui->size144Button
    };

    for (int i = 0; i < 5; ++i) {
        buttons[i]->setEnabled(radioButtons[i]);
    }
}

int MapGeneratorApp::getSelectedScenarioSize()
{
    QRadioButton *buttons[5] = {
        ui->size48Button,
        ui->size72Button,
        ui->size96Button,
        ui->size120Button,
        ui->size144Button
    };

    for (int i = 0; i < 5; ++i) {
        auto button = buttons[i];
        if (button->isEnabled() && button->isChecked()) {
            return 48 + i * 24;
        }
    }

    return 0;
}

std::time_t MapGeneratorApp::getScenarioSeed()
{
    // Get user specified seed or use current time
    if (ui->seedEdit->text().isEmpty()) {
        return std::time(nullptr);
    }

    const auto seedText = ui->seedEdit->text();
    bool converted{false};
    std::time_t seed = seedText.toUInt(&converted);

    if (!converted) {
        return std::time(nullptr);
    }

    return seed;
}

void MapGeneratorApp::updatePreviewImages()
{
    const size_t width = static_cast<size_t>(options.size);
    const size_t height = static_cast<size_t>(options.size);
    const auto pixelsTotal{width * height};

    std::vector<RgbColor> pixels(pixelsTotal);
    std::vector<RgbColor> pixels2(pixelsTotal);

    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            Position pos{static_cast<int>(i), static_cast<int>(j)};

            // clang-format off
            static const RgbColor colors[] = {
                RgbColor{255, 0, 0},        // red
                RgbColor{0, 255, 0},        // green
                RgbColor{0, 0, 255},        // blue
                RgbColor{255, 255, 255},    // white
                RgbColor{0, 0, 0},          // black
                RgbColor{127, 127, 127},    // gray
                RgbColor{255, 255, 0},      // yellow
                RgbColor{0, 255, 255},      // cyan
                RgbColor{255, 0, 255},      // magenta
                RgbColor{255, 153, 0},      // orange
                RgbColor{127, 0, 0},        // dark red
                RgbColor{0, 127, 0},        // dark green
                RgbColor{0, 0, 127},        // dark blue
                RgbColor{64, 64, 64},       // dark gray
                RgbColor{127, 127, 0},      // dark yellow
                RgbColor{0, 127, 127},      // dark cyan
                RgbColor{127, 0, 127},      // violet
                RgbColor{127, 64, 0},       // brown
            };
            // clang-format on

            const std::size_t index = i + width * j;

            const auto zoneId{generator->zoneColoring[generator->posToIndex(pos)]};

            if (zoneId < std::size(colors)) {
                pixels[index] = colors[zoneId];
            }
            else {
                std::uint8_t c = 32 + 10 * (zoneId - std::size(colors));
                pixels[index] = RgbColor(c, c, c);
            }

            auto& tile{generator->tiles[generator->posToIndex(pos)]};

            if (tile.isRoad()) {
                pixels2[index] = RgbColor(175, 175, 175); // grey
            } else if (tile.isUsed()) {
                pixels2[index] = RgbColor(237, 177, 100); // yellow
            } else if (tile.isBlocked()) {
                pixels2[index] = RgbColor(255, 0, 0); // red
            } else if (tile.isFree()) {
                pixels2[index] = RgbColor(255, 255, 255); // white
            } else if (tile.isPossible()) {
                pixels2[index] = RgbColor(255, 179, 185); // pink
            } else {
                pixels2[index] = RgbColor(0, 0, 0); // black for all other
            }
        }
    }

    auto zonesData = reinterpret_cast<uchar*>(pixels.data());
    QImage zonesImage(zonesData, options.size, options.size, QImage::Format_RGB888);
    auto zonesPixmap = QPixmap::fromImage(zonesImage);

    ui->zonesImage->setPixmap(zonesPixmap.scaled(288, 288));

    auto contentData = reinterpret_cast<uchar*>(pixels2.data());
    QImage contentImage(contentData, options.size, options.size, QImage::Format_RGB888);
    auto contentPixmap = QPixmap::fromImage(contentImage);

    ui->contentsImage->setPixmap(contentPixmap.scaled(288, 288));
}

void MapGeneratorApp::on_scenarioTemplateButton_clicked()
{
    const QString filepath = QFileDialog::getOpenFileName(this,
                                                          tr("Open template file"),
                                                          "",
                                                          tr("Templates (*.lua)"));
    if (filepath.isEmpty()) {
        // Canceled by user
        return;
    }

    const std::filesystem::path templatePath{filepath.toStdString()};
    readTemplateAndUpdateUi(templatePath);
}

void MapGeneratorApp::on_generateButton_clicked()
{
    const auto scenarioSize = getSelectedScenarioSize();
    if (!scenarioSize) {
        QMessageBox::critical(this, tr("Error"), tr("Wrong scenario size"));
        return;
    }


    const auto seed = getScenarioSeed();
    const auto seedString = std::to_string(seed);

    // Create options
    options = MapGenOptions();
    options.mapTemplate = mapTemplate.get();
    options.name = std::string{"random map "} + seedString;
    options.description = std::string{"Random map based on template '"}
            + options.mapTemplate->name
            + std::string{"'. Seed: "} + seedString;
    options.size = scenarioSize;
    // Create generator
    generator = std::make_unique<MapGenerator>(options, seed);
    // Remember radio button states
    rememberRadioButtonStates();
    // Disable buttons
    disableButtons(true);
    // Start generation in another thread, wait for signal
    auto thread = new MapGeneratorThread(generator.get(), this);

    connect(thread, &MapGeneratorThread::mapGenerated, this, &MapGeneratorApp::onScenarioMapGenerated);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void MapGeneratorApp::on_saveScenarioButtom_clicked()
{
    auto filePath = QFileDialog::getSaveFileName(this,
                                                 tr("Save scenario"),
                                                 "",
                                                 tr("Disciples II scenarios (*.sg)"));
    if (filePath.isEmpty()) {
        return;
    }

    // Remember radio button states
    rememberRadioButtonStates();
    // Disable buttons
    disableButtons(true);

    const std::filesystem::path scenarioPath{filePath.toStdString()};
    scenario->serialize(scenarioPath);

    enableButtons();
    ui->saveScenarioButtom->setEnabled(true);
}

void MapGeneratorApp::on_scenarioTemplateButtonReload_clicked()
{
    if (templateFilePath.empty()) {
        // How we are ended up with enabled button, but no valid path?
        return;
    }

    readTemplateAndUpdateUi(templateFilePath);
}


void MapGeneratorApp::on_roadsSpinBox_valueChanged(int roadsPercentage)
{
    if (!mapTemplate) {
        // How we are ended up with enabled roads spin box, but no valid map template?
        assert(false);
        return;
    }

    mapTemplate->roads = roadsPercentage;
}


void MapGeneratorApp::on_forestSpinBox_valueChanged(int forestPercentage)
{
    if (!mapTemplate) {
        // How we are ended up with enabled forest spin box, but no valid map template?
        assert(false);
        return;
    }

    mapTemplate->forest = forestPercentage;
}


void MapGeneratorApp::on_goldSpinBox_valueChanged(int gold)
{
    if (!mapTemplate) {
        // How we are ended up with enabled gold spin box, but no valid map template?
        assert(false);
        return;
    }

    mapTemplate->startingGold = gold;
}
