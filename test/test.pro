QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

#DESTDIR  = $$PWD/../bin
TARGET = cpphelper

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += ../cpphelper/

INCLUDEPATH += $$PWD/../mydll
DEPENDPATH += $$PWD/../mydll

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../mydll/release/ -lmydll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../mydll/debug/ -lmydll
else:unix: LIBS += -L$$OUT_PWD/../mydll/ -lmydll

SOURCES += \
        main.cpp \
        test.cpp

HEADERS += \
    ../cpphelper/command.hpp \
    ../cpphelper/library.hpp \
    test.h
