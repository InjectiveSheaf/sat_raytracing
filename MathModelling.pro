QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Calculation/calculation.cpp \
    Graphics/tracer.cpp \
    drawwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    sgp4lib/CoordGeodetic.cc \
    sgp4lib/CoordTopocentric.cc \
    sgp4lib/DateTime.cc \
    sgp4lib/DecayedException.cc \
    sgp4lib/Eci.cc \
    sgp4lib/Globals.cc \
    sgp4lib/Observer.cc \
    sgp4lib/OrbitalElements.cc \
    sgp4lib/SGP4.cc \
    sgp4lib/SatelliteException.cc \
    sgp4lib/SolarPosition.cc \
    sgp4lib/TimeSpan.cc \
    sgp4lib/Tle.cc \
    sgp4lib/TleException.cc \
    sgp4lib/Util.cc \
    sgp4lib/Vector.cc

HEADERS += \
    Calculation/calculation.h \
    Graphics/geometricObjects.h \
    Graphics/tracer.h \
    drawwidget.h \
    mainwindow.h \
    sgp4lib/CoordGeodetic.h \
    sgp4lib/CoordTopocentric.h \
    sgp4lib/DateTime.h \
    sgp4lib/DecayedException.h \
    sgp4lib/Eci.h \
    sgp4lib/Globals.h \
    sgp4lib/Observer.h \
    sgp4lib/OrbitalElements.h \
    sgp4lib/SGP4.h \
    sgp4lib/SatelliteException.h \
    sgp4lib/SolarPosition.h \
    sgp4lib/TimeSpan.h \
    sgp4lib/Tle.h \
    sgp4lib/TleException.h \
    sgp4lib/Util.h \
    sgp4lib/Vector.h

FORMS += \
    mainwindow.ui

QMAKE_CXXFLAGS += -fopenmp

LIBS += -fopenmp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
