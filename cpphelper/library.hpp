#pragma once

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

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

namespace CppHelper
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
}
