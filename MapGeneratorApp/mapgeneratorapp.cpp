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
#include <QDebug>
#include <QComboBox>

static const char* getRaceLabel(rsg::RaceType race)
{
    using namespace rsg;

    switch (race) {
    case RaceType::Human: return "empire.png";
    case RaceType::Dwarf: return "dwarf.png";
    case RaceType::Undead: return "undead.png";
    case RaceType::Heretic: return "heretic.png";
    case RaceType::Elf: return "elf.png";
    default:
    case RaceType::Neutral:
    case RaceType::Random: return "random.png";
    }
}

static int getComboBoxIndex(rsg::RaceType race)
{
    using namespace rsg;

    switch (race) {
    case RaceType::Human: return 1;
    case RaceType::Dwarf: return 2;
    case RaceType::Undead: return 3;
    case RaceType::Heretic: return 4;
    case RaceType::Elf: return 5;
    default:
    case RaceType::Neutral:
    case RaceType::Random: return 0;
    }
}

static rsg::RaceType comboBoxIndexToRace(int index)
{
    using namespace rsg;

    switch (index) {
    case 1: return RaceType::Human;
    case 2: return RaceType::Dwarf;
    case 3: return RaceType::Undead;
    case 4: return RaceType::Heretic;
    case 5: return RaceType::Elf;
    default:
    case 0:
        return RaceType::Random;
    }
}

MapGeneratorApp::MapGeneratorApp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapGeneratorApp)
{
    ui->setupUi(this);

    ui->aboutLabel->setText(tr("v %1, mak").arg(VER_PRODUCTVERSION_STR));

    connect(&seedPlaceholderTimer, &QTimer::timeout, this, &MapGeneratorApp::seedPlaceholderUpdate);

    connect(ui->race1ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onRaceSelected(int)));
    connect(ui->race2ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onRaceSelected(int)));
    connect(ui->race3ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onRaceSelected(int)));
    connect(ui->race4ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onRaceSelected(int)));

    disableButtons();
}

MapGeneratorApp::~MapGeneratorApp()
{
    delete ui;
}

void MapGeneratorApp::onScenarioMapGenerated(rsg::Map *scenarioMap, const QString &error)
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

void MapGeneratorApp::onRaceSelected(int comboBoxIndex)
{
    QComboBox* comboBoxes[4] = {
        ui->race1ComboBox,
        ui->race2ComboBox,
        ui->race3ComboBox,
        ui->race4ComboBox
    };

    QLabel* labels[4] = {
        ui->race1Label,
        ui->race2Label,
        ui->race3Label,
        ui->race4Label
    };

    using namespace rsg;

    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
    const RaceType selectedRace = comboBoxIndexToRace(comboBoxIndex);

    for (int i = 0; i < 4; ++i) {
        QComboBox* other = comboBoxes[i];

        if (comboBox == other) {
            // Update corresponding label according to index
            qDebug() << "ComboBox" << i + 1 << "changed to index:" << comboBoxIndex;

            const char* labelName = getRaceLabel(selectedRace);
            labels[i]->setPixmap(QPixmap(QString(":/images/%1").arg(labelName)));
        }
        else {
            // Make sure there are no duplicates in playable races
            if (selectedRace == RaceType::Random) {
                // Multiple random races are accepted
                continue;
            }

            const RaceType otherRace = comboBoxIndexToRace(other->currentIndex());
            if (otherRace == selectedRace) {
                // Other combo box have this race already selected, revert it to random
                other->setCurrentIndex(getComboBoxIndex(RaceType::Random));
            }
        }
    }
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

    return rsg::readGameInfo(gameFolder);
}

void MapGeneratorApp::readTemplateAndUpdateUi(const std::filesystem::path& templatePath)
{
    try {
        rsg::MapTemplate* tmplt = rsg::readTemplateSettings(templatePath);

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

    auto& settings = mapTemplate->settings;
    // Allow user to change roads, forests and starting gold
    ui->goldSpinBox->setValue(settings.startingGold);
    ui->goldSpinBox->setEnabled(true);

    ui->roadsSpinBox->setValue(settings.roads);
    ui->roadsSpinBox->setEnabled(true);

    ui->forestSpinBox->setValue(settings.forest);
    ui->forestSpinBox->setEnabled(true);

    // Allow user to select scenario size
    updateRadioButtons();
    updateRaceButtons();
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
    disableRaceButtons(forGeneration);
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

void MapGeneratorApp::disableRaceButtons(bool forGeneration)
{
    QCheckBox* checkBoxes[4] = {
        ui->race1CheckBox,
        ui->race2CheckBox,
        ui->race3CheckBox,
        ui->race4CheckBox
    };

    QComboBox* comboBoxes[4] = {
        ui->race1ComboBox,
        ui->race2ComboBox,
        ui->race3ComboBox,
        ui->race4ComboBox
    };

    for (int i = 0; i < 4; ++i) {
        comboBoxes[i]->setEnabled(false);
        checkBoxes[i]->setEnabled(false);

        if (!forGeneration) {
            comboBoxes[i]->setCurrentIndex(-1);
            checkBoxes[i]->setChecked(false);
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
    restoreButtonStates();
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

    const auto startIndex = mapTemplate->settings.sizeMin / 24 - 2;
    const auto endIndex = mapTemplate->settings.sizeMax / 24 - 2;

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

void MapGeneratorApp::updateRaceButtons()
{
    // Disable them first
    disableRaceButtons();

    QCheckBox* checkBoxes[4] = {
        ui->race1CheckBox,
        ui->race2CheckBox,
        ui->race3CheckBox,
        ui->race4CheckBox
    };

    QComboBox* comboBoxes[4] = {
        ui->race1ComboBox,
        ui->race2ComboBox,
        ui->race3ComboBox,
        ui->race4ComboBox
    };

    // Enable according to maxPlayers
    const int maxPlayers = mapTemplate->settings.maxPlayers;

    for (int i = 0; i < maxPlayers; ++i) {
        //checkBoxes[i]->setEnabled(true);
        // Check by default, for convenience
        checkBoxes[i]->setChecked(true);

        comboBoxes[i]->setEnabled(true);
        // Choose random race by default
        comboBoxes[i]->setCurrentIndex(getComboBoxIndex(rsg::RaceType::Random));
    }
}

void MapGeneratorApp::restoreRaceButtonStates()
{
    /*QCheckBox* checkBoxes[4] = {
        ui->race1CheckBox,
        ui->race2CheckBox,
        ui->race3CheckBox,
        ui->race4CheckBox
    };*/

    QComboBox* comboBoxes[4] = {
        ui->race1ComboBox,
        ui->race2ComboBox,
        ui->race3ComboBox,
        ui->race4ComboBox
    };

    // Enable according to maxPlayers
    const int maxPlayers = mapTemplate->settings.maxPlayers;

    for (int i = 0; i < maxPlayers; ++i) {
        //checkBoxes[i]->setEnabled(true);
        comboBoxes[i]->setEnabled(true);
    }
}

void MapGeneratorApp::restoreButtonStates()
{
    restoreRadioButtonStates();
    restoreRaceButtonStates();
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
        QRadioButton* button = buttons[i];
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
    using namespace rsg;

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

void MapGeneratorApp::getSelectedRaces(std::vector<rsg::RaceType>& races, int maxPlayers)
{
    /*QCheckBox* checkBoxes[4] = {
        ui->race1CheckBox,
        ui->race2CheckBox,
        ui->race3CheckBox,
        ui->race4CheckBox
    };*/

    QComboBox* comboBoxes[4] = {
        ui->race1ComboBox,
        ui->race2ComboBox,
        ui->race3ComboBox,
        ui->race4ComboBox
    };

    races.clear();

    for (int i = 0; i < maxPlayers; ++i) {
        //if (checkBoxes[i]->isChecked()) {
            races.push_back(comboBoxIndexToRace(comboBoxes[i]->currentIndex()));
        //}
    }
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

    using namespace rsg;

    const auto seed = getScenarioSeed();
    const auto seedString = std::to_string(seed);

    auto& settings = mapTemplate->settings;
    settings.size = scenarioSize;
    getSelectedRaces(settings.races, settings.maxPlayers);

    // Create options
    options = MapGenOptions();
    options.mapTemplate = mapTemplate.get();
    options.name = std::string{"Random scenario "} + seedString;
    options.description = std::string{"Random scenario based on template '"}
            + settings.name
            + std::string{"'. Seed: "} + seedString
            + ". Starting gold: " + std::to_string(settings.startingGold)
            + ". Roads: " + std::to_string(settings.roads)
            + "%. Forest: " + std::to_string(settings.forest)
            + "%.";
    options.size = settings.size;
    // Create generator
    generator = std::make_unique<rsg::MapGenerator>(options, seed);

    try {
        // Cleanup previous contents, if any
        mapTemplate->contents = MapTemplateContents();

        settings.replaceRandomRaces(generator->randomGenerator);
        // Generate new contents according to user settings
        readTemplateContents(*mapTemplate);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Error"), tr("Could not generate template contents. Reason: %1").arg(e.what()));
    }

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
    scenario->author = std::string("mss32 rsg v ") + VER_PRODUCTVERSION_STR;
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

    mapTemplate->settings.roads = roadsPercentage;
}


void MapGeneratorApp::on_forestSpinBox_valueChanged(int forestPercentage)
{
    if (!mapTemplate) {
        // How we are ended up with enabled forest spin box, but no valid map template?
        assert(false);
        return;
    }

    mapTemplate->settings.forest = forestPercentage;
}


void MapGeneratorApp::on_goldSpinBox_valueChanged(int gold)
{
    if (!mapTemplate) {
        // How we are ended up with enabled gold spin box, but no valid map template?
        assert(false);
        return;
    }

    mapTemplate->settings.startingGold = gold;
}
