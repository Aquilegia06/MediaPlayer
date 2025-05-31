QT += core gui multimedia widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    pmadia_ru_BY.ts

CONFIG += lrelease
CONFIG += embed_translations
