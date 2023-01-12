#-------------------------------------------------
#
# Project created by QtCreator 2022-07-28T19:20:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rsg
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += \
        QT_DEPRECATED_WARNINGS \
        _USE_MATH_DEFINES \
        _CRT_SECURE_NO_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

INCLUDEPATH += \
        ../ \
        ../ScenarioGenerator/src \
        ../ScenarioGenerator/src/scenario \
        ../sol2/single/include/ \
        ../lua/ \
        ../GSL/include/

SOURCES += \
        ../ScenarioGenerator/src/currency.cpp \
        ../ScenarioGenerator/src/decoration.cpp \
        ../ScenarioGenerator/src/gameinfo.cpp \
        ../ScenarioGenerator/src/generatorsettings.cpp \
        ../ScenarioGenerator/src/image.cpp \
        ../ScenarioGenerator/src/itempicker.cpp \
        ../ScenarioGenerator/src/landmarkpicker.cpp \
        ../ScenarioGenerator/src/mapgenerator.cpp \
        ../ScenarioGenerator/src/maptemplatereader.cpp \
        ../ScenarioGenerator/src/midgardid.cpp \
        ../ScenarioGenerator/src/mqdb.cpp \
        ../ScenarioGenerator/src/scenario/bag.cpp \
        ../ScenarioGenerator/src/scenario/capital.cpp \
        ../ScenarioGenerator/src/scenario/crystal.cpp \
        ../ScenarioGenerator/src/scenario/diplomacy.cpp \
        ../ScenarioGenerator/src/scenario/fog.cpp \
        ../ScenarioGenerator/src/scenario/fortification.cpp \
        ../ScenarioGenerator/src/scenario/group.cpp \
        ../ScenarioGenerator/src/scenario/inventory.cpp \
        ../ScenarioGenerator/src/scenario/item.cpp \
        ../ScenarioGenerator/src/scenario/knownspells.cpp \
        ../ScenarioGenerator/src/scenario/landmark.cpp \
        ../ScenarioGenerator/src/scenario/mage.cpp \
        ../ScenarioGenerator/src/scenario/map.cpp \
        ../ScenarioGenerator/src/scenario/mapblock.cpp \
        ../ScenarioGenerator/src/scenario/mercenary.cpp \
        ../ScenarioGenerator/src/scenario/merchant.cpp \
        ../ScenarioGenerator/src/scenario/midgardmap.cpp \
        ../ScenarioGenerator/src/scenario/mountains.cpp \
        ../ScenarioGenerator/src/scenario/plan.cpp \
        ../ScenarioGenerator/src/scenario/player.cpp \
        ../ScenarioGenerator/src/scenario/playerbuildings.cpp \
        ../ScenarioGenerator/src/scenario/questlog.cpp \
        ../ScenarioGenerator/src/scenario/road.cpp \
        ../ScenarioGenerator/src/scenario/ruin.cpp \
        ../ScenarioGenerator/src/scenario/scenarioinfo.cpp \
        ../ScenarioGenerator/src/scenario/scenariovariables.cpp \
        ../ScenarioGenerator/src/scenario/site.cpp \
        ../ScenarioGenerator/src/scenario/spellcast.cpp \
        ../ScenarioGenerator/src/scenario/spelleffects.cpp \
        ../ScenarioGenerator/src/scenario/stack.cpp \
        ../ScenarioGenerator/src/scenario/stackdestroyed.cpp \
        ../ScenarioGenerator/src/scenario/subrace.cpp \
        ../ScenarioGenerator/src/scenario/talismancharges.cpp \
        ../ScenarioGenerator/src/scenario/turnsummary.cpp \
        ../ScenarioGenerator/src/scenario/unit.cpp \
        ../ScenarioGenerator/src/scenario/village.cpp \
        ../ScenarioGenerator/src/serializer.cpp \
        ../ScenarioGenerator/src/spellpicker.cpp \
        ../ScenarioGenerator/src/templatezone.cpp \
        ../ScenarioGenerator/src/textconvert.cpp \
        ../ScenarioGenerator/src/texts.cpp \
        ../ScenarioGenerator/src/unitpicker.cpp \
        ../ScenarioGenerator/src/zoneplacer.cpp \
        ../dbf.cpp \
        ../lua/lapi.c \
        ../lua/lauxlib.c \
        ../lua/lbaselib.c \
        ../lua/lcode.c \
        ../lua/lcorolib.c \
        ../lua/lctype.c \
        ../lua/ldblib.c \
        ../lua/ldebug.c \
        ../lua/ldo.c \
        ../lua/ldump.c \
        ../lua/lfunc.c \
        ../lua/lgc.c \
        ../lua/linit.c \
        ../lua/liolib.c \
        ../lua/llex.c \
        ../lua/lmathlib.c \
        ../lua/lmem.c \
        ../lua/loadlib.c \
        ../lua/lobject.c \
        ../lua/lopcodes.c \
        ../lua/loslib.c \
        ../lua/lparser.c \
        ../lua/lstate.c \
        ../lua/lstring.c \
        ../lua/lstrlib.c \
        ../lua/ltable.c \
        ../lua/ltablib.c \
        ../lua/ltm.c \
        ../lua/lundump.c \
        ../lua/lutf8lib.c \
        ../lua/lvm.c \
        ../lua/lzio.c \
        ../standalonegameinfo.cpp \
        main.cpp \
        mapgeneratorapp.cpp \
        mapgeneratorthread.cpp

HEADERS += \
        ../ScenarioGenerator/src/aipriority.h \
        ../ScenarioGenerator/src/containers.h \
        ../ScenarioGenerator/src/currency.h \
        ../ScenarioGenerator/src/decoration.h \
        ../ScenarioGenerator/src/enums.h \
        ../ScenarioGenerator/src/exceptions.h \
        ../ScenarioGenerator/src/gameinfo.h \
        ../ScenarioGenerator/src/generatorsettings.h \
        ../ScenarioGenerator/src/image.h \
        ../ScenarioGenerator/src/iteminfo.h \
        ../ScenarioGenerator/src/itempicker.h \
        ../ScenarioGenerator/src/landmarkinfo.h \
        ../ScenarioGenerator/src/landmarkpicker.h \
        ../ScenarioGenerator/src/mapgenerator.h \
        ../ScenarioGenerator/src/maptemplate.h \
        ../ScenarioGenerator/src/maptemplatereader.h \
        ../ScenarioGenerator/src/midgardid.h \
        ../ScenarioGenerator/src/mqdb.h \
        ../ScenarioGenerator/src/picker.h \
        ../ScenarioGenerator/src/position.h \
        ../ScenarioGenerator/src/raceinfo.h \
        ../ScenarioGenerator/src/randomgenerator.h \
        ../ScenarioGenerator/src/scenario/bag.h \
        ../ScenarioGenerator/src/scenario/capital.h \
        ../ScenarioGenerator/src/scenario/crystal.h \
        ../ScenarioGenerator/src/scenario/diplomacy.h \
        ../ScenarioGenerator/src/scenario/fog.h \
        ../ScenarioGenerator/src/scenario/fortification.h \
        ../ScenarioGenerator/src/scenario/group.h \
        ../ScenarioGenerator/src/scenario/inventory.h \
        ../ScenarioGenerator/src/scenario/item.h \
        ../ScenarioGenerator/src/scenario/knownspells.h \
        ../ScenarioGenerator/src/scenario/landmark.h \
        ../ScenarioGenerator/src/scenario/mage.h \
        ../ScenarioGenerator/src/scenario/map.h \
        ../ScenarioGenerator/src/scenario/mapblock.h \
        ../ScenarioGenerator/src/scenario/mapelement.h \
        ../ScenarioGenerator/src/scenario/mercenary.h \
        ../ScenarioGenerator/src/scenario/merchant.h \
        ../ScenarioGenerator/src/scenario/midgardmap.h \
        ../ScenarioGenerator/src/scenario/mountains.h \
        ../ScenarioGenerator/src/scenario/plan.h \
        ../ScenarioGenerator/src/scenario/player.h \
        ../ScenarioGenerator/src/scenario/playerbuildings.h \
        ../ScenarioGenerator/src/scenario/questlog.h \
        ../ScenarioGenerator/src/scenario/road.h \
        ../ScenarioGenerator/src/scenario/ruin.h \
        ../ScenarioGenerator/src/scenario/scenarioinfo.h \
        ../ScenarioGenerator/src/scenario/scenarioobject.h \
        ../ScenarioGenerator/src/scenario/scenariovariables.h \
        ../ScenarioGenerator/src/scenario/site.h \
        ../ScenarioGenerator/src/scenario/spellcast.h \
        ../ScenarioGenerator/src/scenario/spelleffects.h \
        ../ScenarioGenerator/src/scenario/stack.h \
        ../ScenarioGenerator/src/scenario/stackdestroyed.h \
        ../ScenarioGenerator/src/scenario/subrace.h \
        ../ScenarioGenerator/src/scenario/talismancharges.h \
        ../ScenarioGenerator/src/scenario/trainer.h \
        ../ScenarioGenerator/src/scenario/turnsummary.h \
        ../ScenarioGenerator/src/scenario/unit.h \
        ../ScenarioGenerator/src/scenario/village.h \
        ../ScenarioGenerator/src/serializer.h \
        ../ScenarioGenerator/src/spellinfo.h \
        ../ScenarioGenerator/src/spellpicker.h \
        ../ScenarioGenerator/src/stb_image_write.h \
        ../ScenarioGenerator/src/templatezone.h \
        ../ScenarioGenerator/src/textconvert.h \
        ../ScenarioGenerator/src/texts.h \
        ../ScenarioGenerator/src/tileinfo.h \
        ../ScenarioGenerator/src/unitinfo.h \
        ../ScenarioGenerator/src/unitpicker.h \
        ../ScenarioGenerator/src/vposition.h \
        ../ScenarioGenerator/src/zoneid.h \
        ../ScenarioGenerator/src/zoneoptions.h \
        ../ScenarioGenerator/src/zoneplacer.h \
        ../dbf.h \
        ../lua/lapi.h \
        ../lua/lauxlib.h \
        ../lua/lcode.h \
        ../lua/lctype.h \
        ../lua/ldebug.h \
        ../lua/ldo.h \
        ../lua/lfunc.h \
        ../lua/lgc.h \
        ../lua/ljumptab.h \
        ../lua/llex.h \
        ../lua/llimits.h \
        ../lua/lmem.h \
        ../lua/lobject.h \
        ../lua/lopcodes.h \
        ../lua/lopnames.h \
        ../lua/lparser.h \
        ../lua/lprefix.h \
        ../lua/lstate.h \
        ../lua/lstring.h \
        ../lua/ltable.h \
        ../lua/ltm.h \
        ../lua/lua.h \
        ../lua/lua.hpp \
        ../lua/luaconf.h \
        ../lua/lualib.h \
        ../lua/lundump.h \
        ../lua/lvm.h \
        ../lua/lzio.h \
        ../standalonegameinfo.h \
        ../standaloneiteminfo.h \
        ../standalonelandmarkinfo.h \
        ../standaloneraceinfo.h \
        ../standalonespellinfo.h \
        ../standaloneunitinfo.h \
        mapgeneratorapp.h \
        mapgeneratorthread.h \
        version.h

FORMS += \
        mapgeneratorapp.ui

# Application icon
#win32: RC_ICONS += app.ico

# Resources script
win32: RC_FILE = resources.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
