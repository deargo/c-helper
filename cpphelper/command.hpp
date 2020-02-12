#pragma once

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <functional>
#include <type_traits>
#include <iostream>

using namespace std;

namespace CppHelper
{

template<typename Ret=void>
struct CCommand
{
private:
    std::function < Ret()> m_func;

public:
    //����function����������lamda����ͨ�����İ�װ��
    template< class Func, class... Args, class = typename std::enable_if<!std::is_member_function_pointer<Func>::value>::type>
    void Wrap(Func && func, Args && ... args)
    {
        m_func = [&]{return func(args...); };
    }
    //���ܳ�����Ա�����İ�װ��
    template<class CObj, class... DArgs, class PObj, class... Args>
    void Wrap(Ret(CObj::*func)(DArgs...) const, PObj && pObj, Args && ... args)
    {
        m_func = [&, func]{return (*pObj.*func)( args...); };
    }

    //���ܷǳ�����Ա�����İ�װ��
    template<class CObj, class... DArgs, class PObj, class... Args>
    void Wrap(Ret(CObj::*func)(DArgs...), PObj && pObj, Args && ... args)
    {
        m_func = [&, func]{return (*pObj.*func)( args...); };
    }

    Ret Excecute()
    {
        return m_func();
    }
};
}
