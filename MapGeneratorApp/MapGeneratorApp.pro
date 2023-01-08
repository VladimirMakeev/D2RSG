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
        ../sol2/single/include/ \
        ../lua/ \
        ../GSL/include/

SOURCES += \
        ../bag.cpp \
        ../capital.cpp \
        ../crystal.cpp \
        ../currency.cpp \
        ../dbf.cpp \
        ../decoration.cpp \
        ../diplomacy.cpp \
        ../fog.cpp \
        ../fortification.cpp \
        ../gameinfo.cpp \
        ../generatorsettings.cpp \
        ../group.cpp \
        ../image.cpp \
        ../inventory.cpp \
        ../item.cpp \
        ../itempicker.cpp \
        ../knownspells.cpp \
        ../landmark.cpp \
        ../landmarkpicker.cpp \
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
        ../mage.cpp \
        ../map.cpp \
        ../mapblock.cpp \
        ../mapgenerator.cpp \
        ../maptemplatereader.cpp \
        ../mercenary.cpp \
        ../merchant.cpp \
        ../midgardid.cpp \
        ../midgardmap.cpp \
        ../mountains.cpp \
        ../mqdb.cpp \
        ../plan.cpp \
        ../player.cpp \
        ../playerbuildings.cpp \
        ../questlog.cpp \
        ../road.cpp \
        ../ruin.cpp \
        ../scenarioinfo.cpp \
        ../scenariovariables.cpp \
        ../serializer.cpp \
        ../site.cpp \
        ../spellcast.cpp \
        ../spelleffects.cpp \
        ../spellpicker.cpp \
        ../stack.cpp \
        ../stackdestroyed.cpp \
        ../subrace.cpp \
        ../talismancharges.cpp \
        ../templatezone.cpp \
        ../textconvert.cpp \
        ../texts.cpp \
        ../turnsummary.cpp \
        ../unit.cpp \
        ../unitpicker.cpp \
        ../village.cpp \
        ../zoneplacer.cpp \
        main.cpp \
        mapgeneratorapp.cpp \
        mapgeneratorthread.cpp

HEADERS += \
        ../aipriority.h \
        ../bag.h \
        ../capital.h \
        ../containers.h \
        ../crystal.h \
        ../currency.h \
        ../dbf.h \
        ../decoration.h \
        ../diplomacy.h \
        ../enums.h \
        ../exceptions.h \
        ../fog.h \
        ../fortification.h \
        ../gameinfo.h \
        ../generatorsettings.h \
        ../group.h \
        ../image.h \
        ../inventory.h \
        ../item.h \
        ../itempicker.h \
        ../knownspells.h \
        ../landmark.h \
        ../landmarkpicker.h \
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
        ../mage.h \
        ../map.h \
        ../mapblock.h \
        ../mapelement.h \
        ../mapgenerator.h \
        ../maptemplate.h \
        ../maptemplatereader.h \
        ../mercenary.h \
        ../merchant.h \
        ../midgardid.h \
        ../midgardmap.h \
        ../mountains.h \
        ../mqdb.h \
        ../plan.h \
        ../player.h \
        ../playerbuildings.h \
        ../position.h \
        ../questlog.h \
        ../randomgenerator.h \
        ../road.h \
        ../ruin.h \
        ../scenarioinfo.h \
        ../scenarioobject.h \
        ../scenariovariables.h \
        ../serializer.h \
        ../site.h \
        ../spellcast.h \
        ../spelleffects.h \
        ../spellpicker.h \
        ../stack.h \
        ../stackdestroyed.h \
        ../stb_image_write.h \
        ../subrace.h \
        ../talismancharges.h \
        ../templatezone.h \
        ../textconvert.h \
        ../texts.h \
        ../tileinfo.h \
        ../trainer.h \
        ../turnsummary.h \
        ../unit.h \
        ../unitpicker.h \
        ../village.h \
        ../vposition.h \
        ../zoneid.h \
        ../zoneoptions.h \
        ../zoneplacer.h \
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
