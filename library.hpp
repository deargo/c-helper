#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#ifdef _WIN32
#   include <Windows.h>
#   include <libloaderapi.h>
#   pragma warning(disable:4297)
#else
#   include <dlfcn.h>
#endif

using namespace std;

namespace Helper
{

//原文链接：https://blog.csdn.net/aguoxin/article/details/104265711
//参考资料：《深入应用C++11：代码优化与工程级应用》 https://www.cnblogs.com/qicosmos/p/4551499.html
class 	CLibrary
{
public:    
#ifdef _WIN32
    typedef HMODULE Object;
    typedef HANDLE  Symbol;
#else
    typedef void* Object;
    typedef void* Symbol;
#endif
    CLibrary():m_lib(nullptr)
    {
    }
    CLibrary(const string& libarayPath):m_lib(nullptr)
    {
        load(libarayPath);
    }
    virtual ~CLibrary()
    {
        unload();
    }

    bool load(const string& libarayPath)
    {
        unload();
#ifdef _WIN32
        m_lib = LoadLibraryA(libarayPath.c_str());
#else
        m_lib = dlopen(libarayPath.c_str(), RTLD_NOW);
#endif
        return nullptr != m_lib;
    }

    bool unload()
    {
        m_map.clear();
        if (m_lib == nullptr)
        {
            return true;
        }
#ifdef _WIN32
        FreeLibrary(m_lib);
#else
        dlclose(m_lib);
#endif
        if (!m_lib)
        {
            return false;
        }
        m_lib = nullptr;
        return true;
    }

    Object getObject()
    {
        return m_lib;
    }

    Symbol getSymbol(const string& funcName)
    {
#ifdef _WIN32
        Symbol addr = GetProcAddress(m_lib, funcName.c_str());
#else
        Symbol addr = dlsym(m_lib, funcName.c_str());
        if (NULL != dlerror())
        {
            addr = nullptr;
        }
#endif
        return addr;
    }

    bool exist(const string& funcName)
    {
        Symbol symbol = getSymbol(funcName);
        return nullptr != symbol;
    }

    template <typename T>
    std::function<T> getFunction(const string& funcName)
    {
        auto it = m_map.find(funcName);
        if (it == m_map.end())
        {
            Symbol symbol = getSymbol(funcName.c_str());
            if (!symbol)
            {
                return nullptr;
            }
            m_map.insert(std::make_pair(funcName, symbol));
            it = m_map.find(funcName);
        }
        return std::function<T>((T*) (it->second));
    }

    template <typename T, typename... Args>
    typename std::result_of<std::function<T>(Args...)>::type execute(const string& funcName, Args&&... args)
    {
        auto func = getFunction<T>(funcName);
        if (func == nullptr)
        {
            throw string("can not find this function: ") + funcName;
        }
        return func(std::forward<Args>(args)...);
    }

private:
    Object m_lib;
    std::unordered_map<string, Symbol> m_map;
};

/*
//myDll.h
#pragma once

#ifdef _WIN32
#   ifdef EXPORT_API
#       define EXPORT_API __declspec(dllexport)
#   else
#       define EXPORT_API __declspec(dllimport)
#   endif
#else
#   define EXPORT_API
#endif //WIN32

extern "C" int EXPORT_API Add(int x, int y);
extern "C" int EXPORT_API Max(int x, int y);

//myDll.cpp

#include  "myDll.h"

int Add(int x, int y)
{
    return x + y;
}

int Max(int x, int y)
{
    return x > y ? x : y;
}

动态库vs2019编译：
　　项目->属性->常规->配置类型：动态库(.dll)

动态库linux编译：
　　脚本文件：sh myDll.sh。记得添加-fPIC和-shared参数。
#!/bin/bash
rm -fr  libmydll.so

g++ -std=c++11 -fPIC -shared -o libmydll.so myDll.cpp

if [ -f "libmydll.so" ]; then
        cp libmydll.so ../Test/
fi


动态库测试代码
　　记得动态库路径为绝对路径，或者当前路径。

#include <string>
#include <iostream>
#include "library.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    cout << "enter main..." << endl;

#ifdef _WIN32
    Helper::CLibrary lib("mydll.dll");
#else
    Helper::CLibrary lib("libmydll.so");
#endif
    cout << "dll add exsit: " << lib.exist("add")<< endl;

    auto fmax = lib.getFunction<int(int, int)>("Max");
    cout << "dll fMax: "<<fmax(1,5)<<endl;

    cout << "dll fAdd: "<<lib.execute<int(int, int)>("Add", 5, 8)<< endl;


    cout << "exit main." << endl;
    return 0;
}


测试运行：
　　加载动态库需-ldl和LD_LIBRARY_PATH.

#!/bin/bash
rm -fr test.out

g++ -std=c++11 main.cpp  -L./ -lmydll -ldl -o test.out

if [ -f "test.out" ]; then
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./
        ./test.out
fi

*/
}
