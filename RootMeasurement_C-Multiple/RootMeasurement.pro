TEMPLATE = app

VERSION = 1.0.8

TARGET = RootMeasurement

QT += widgets
!isEmpty(QT.printsupport.name): QT += printsupport

simulator: warning(This example might not fully work on Simulator platform)


HEADERS += activecontour.hpp \
    filters.hpp \
    hausdorff_distance.hpp \
    ac_withoutedges.hpp \
    geodesic_ac.hpp \
    ac_withoutedges_yuv.hpp \
    linked_list.hpp \
    imageviewer.hpp \
    pixmapwidget.hpp \
    scrollarea.hpp

SOURCES += activecontour.cpp \
    hausdorff_distance.cpp \
    filters.cpp \
    ac_withoutedges.cpp \
    ac_withoutedges_yuv.cpp \
    geodesic_ac.cpp \
    main.cpp \
    imageviewer.cpp \
    pixmapwidget.cpp \
    linked_list.tpp \
    scrollarea.cpp

TRANSLATIONS = $$PWD/Ofeli_fr.ts
CODECFORTR = UTF-8
CODECFORSRC = UTF-8

RESOURCES = $$PWD/Ofeli.qrc


win32 : RC_FILE = $$PWD/Ofeli.rc

# this project uses key words nullptr, override, auto
# and headers <random>, <chrono>, <functional> of c++11 standard
CONFIG += c++11

QMAKE_CXXFLAGS_RELEASE += -O3
