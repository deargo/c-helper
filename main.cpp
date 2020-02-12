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

#include <functional>
#include <vector>
#include <algorithm>
#include <set>

#include "library.hpp"

using namespace std;


int main(int argc, char *argv[])
{
#ifdef __USE_QT__
    QCoreApplication a(argc, argv);
#endif
    cout << "enter main..." << endl;
    //////////////////////////////////////////
    
    
    
    
    //////////////////////////////////////////
    cout << "exit main." << endl;
#ifdef __USE_QT__
    return a.exec();
#else
#	ifdef WIN32
    system("pause");
#	endif
    return 0;
#endif
}
