CONFIG -= qt

TEMPLATE = lib
DEFINES += MYDLL_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#TARGET = $$qtLibraryTarget(mydll)
TARGET = mydll

SOURCES += \
    mydll.cpp

HEADERS += \
    mydll.h
