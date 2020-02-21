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
#include "command.hpp"

using namespace std;

namespace CppHelperTest
{
void CLibraryTest();
void CCommandTest();
void CThreadPoolTest();
void CVectorTest();
void CListTest();
}

