#pragma once

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <string>
#ifdef __USE_QT__
#	include <QCoreApplication>
#	include <QTextStream>
#   include <QDebug>
#   define cout qDebug()
#   define endl ""
#else
#	include <iostream>
#endif

#include "library.hpp"

using namespace std;

namespace CppHelper
{
    namespace Test
    {
        void CLibraryTest()
        {
            //记得动态库路径为绝对路径，或者当前路径。linux动态库需-ldl和LD_LIBRARY_PATH.
#ifdef _WIN32
            CppHelper::CLibrary lib("mydll.dll");
#else
            CppHelper::CLibrary lib("libmydll.so");
#endif
            cout << "dll add exsit: " << lib.exist("add") << endl;

            auto fmax = lib.getFunction<int(int, int)>("Max");
            cout << "dll fMax: " << fmax(1, 5) << endl;

            cout << "dll fAdd: " << lib.execute<int(int, int)>("Add", 5, 8) << endl;
        }
    }
}
