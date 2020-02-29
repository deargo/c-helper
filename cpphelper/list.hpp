#pragma once

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include <functional>
#include <assert.h>

namespace CppHelper
{
template<class Type , class Alloc = std::allocator<Type> >
class CList : public std::list<Type,Alloc>{
    using StdList = std::list<Type,Alloc>;

public:
    explicit CList(const Alloc& alloc = Alloc()):StdList(alloc) {}                                          //默认构造函数
    explicit CList(typename CList::size_type n, const Type& val = Type(), const Alloc& alloc = Alloc()):StdList(n,val,alloc){} //容量构造函数
    CList(CList&& other):StdList(std::forward<CList>(other)){ }                                             //拷贝构造函数
    CList(const std::list<Type,Alloc>& vec){ StdList::assign(vec.begin(), vec.end()); }                     //可用std::list初始化
    template <class InitIterator>                                                                           //可用迭代器初始化
    CList (InitIterator first, InitIterator last, const Alloc& alloc = Alloc()):StdList(first,last,alloc){}
    explicit CList(const std::set<Type>& set){ StdList::assign(set.begin(), set.end()); }                   //可用std::set初始化
    explicit CList(const std::vector<Type>& vec){ StdList::assign(vec.begin(), vec.end()); }                //可用std::vector初始化

public:
    operator std::list<Type,Alloc>(){ return *this; }                                                  //类型转换函数，可直接转换为std::list
    bool operator == (const CList& other) const{ return StdList::size()==other.size() && std::equal(StdList::begin(),StdList::end(),other.begin()); }
    bool operator !=(const CList& other) const { return StdList::size()!=other.size() || false == std::equal(StdList::begin(),StdList::end(),other.begin()); }
    Type& operator [](typename CList::size_type index){ assert(index<StdList::size()); typename CList::iterator it = CList::begin(); while(index--) ++it; return *it; }
    const Type& operator [](typename CList::size_type index) const { assert(index<StdList::size());typename CList::const_iterator it = CList::begin(); while(index--) ++it; return *it; }
    CList& operator =(const CList& other){StdList::assign(other.begin(), other.end()); return *this;}  //赋值构造函数
    CList& operator +(const CList& other){ push_back(other); return *this; }
    CList& operator +=(Type&& value){ push_back(std::forward<Type>(value)); return *this; }
    CList& operator +=(const CList& other){ push_back(other); return *this; }
    CList& operator<<(Type&& value){ push_back(std::forward<Type>(value)); return *this; }
    CList& operator<<(const CList& other){ push_back(other); return *this; }
    CList& operator<<(const std::set<Type>& set){ push_back(set); return *this; }
    CList& operator<<(const std::vector<Type>& vec){ push_back(vec); return *this; }

public:
    void append(Type&& value){ push_back(std::forward<Type>(value)); }
    void append(const CList& other){ push_back(other); }
    void append(const std::set<Type>& set){ push_back(set); }
    void append(const std::vector<Type>& vec){ push_back(vec); }

    const Type& at(typename CList::size_type index) const{ return this->operator[](index);  }

    bool contains(Type&& value) const
    {
        return  std::find(StdList::begin(), StdList::end(),std::forward<Type>(value)) != StdList::end();
    }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&& v1, Type&& v2) 的二元谓词函数，v1为element-value，v2为input-value
    bool contains(Type&& value,CompareFunction compareFunction) const
    {
        return  std::find_if(StdList::begin(), StdList::end(),
                             std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)))
                != StdList::end();
    }

    void copy(const CList& other){ CList::assign(other.begin(),other.end());}
    void copy(typename CList::size_type n, Type&& value) {StdList::assign(n,std::forward<Type>(value));}
    template <class CopyIterator>
    void copy(CopyIterator first,CopyIterator last) { CList::assign(first,last); }
    template <class CopyIterator, class Function>
    void copy(CopyIterator first,CopyIterator last, Function func)
    {
        StdList::clear();
        while (first!=last) {
            if(func(*first))
            {
                StdList::emplace_back(*first);
            }
            ++first;
        }
    }
    template <class CopyIterator>
    void copy(CopyIterator first, typename CList::size_type n)
    {
        StdList::clear();
        while (n) {
            StdList::emplace_back(*first);
            --n;
            ++first;
        }
    }

    typename CList::size_type count(Type&& value) const
    {
        return std::count(StdList::begin(), StdList::end(),std::forward<Type>(value));
    }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&& v1, Type&& v2) 的二元谓词函数，v1为element-value，v2为input-value
    typename CList::size_type count(Type&& value,CompareFunction compareFunction) const
    {
        return  std::count_if(StdList::begin(), StdList::end(),
                              std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)));
    }

    bool equal(const CList& other) const { return this->operator==(other); }
    template <class CompareIterator>
    bool equal(CompareIterator compareIterator) const
    {
        return std::equal(StdList::begin(),StdList::end(),compareIterator);
    }
    template <class CompareIterator, class CompareFunction> //需要传入类似 bool (*compareFunction)(Type&& v1, Type&& v2) 的二元谓词函数，v1为element-value，v2为input-value
    bool equal(CompareIterator compareIterator,CompareFunction compareFunction) const
    {
        return std::equal(StdList::begin(),StdList::end(),compareIterator,compareFunction);
    }

    void fill(Type&& value, typename CList::iterator first=StdList::begin(), typename CList::iterator last=StdList::end()) { std::fill(first,last,std::forward<Type>(value)); }
    void fill(typename CList::size_type n, Type&& value, typename CList::iterator first=StdList::begin())  { std::fill_n(first,n,std::forward<Type>(value)); }

    typename CList::iterator find(Type&& value)
    {
        return  std::find(StdList::begin(), StdList::end(),std::forward<Type>(value));
    }
    typename CList::const_iterator find(Type&& value) const
    {
        return  std::find(StdList::begin(), StdList::end(),std::forward<Type>(value));
    }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&& v1, Type&& v2) 的二元谓词函数，v1为element-value，v2为input-value
    typename CList::iterator find(Type&& value,CompareFunction compareFunction)
    {
        return  std::find_if(StdList::begin(), StdList::end(),
                             std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)));
    }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&& v1, Type&& v2) 的二元谓词函数，v1为element-value，v2为input-value
    typename CList::const_iterator find(Type&& value,CompareFunction compareFunction ) const
    {
        return  std::find_if(StdList::begin(), StdList::end(),
                             std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)));
    }

    const Type& first() const{ return *StdList::begin(); }

    template <class Function>
    Function for_each(Function func){ return std::for_each(StdList::begin(), StdList::end(),func); }

    int index(Type&& value) const {
        for (int idx = 0; idx <(int)StdList::size();++idx){
            if (this->operator[](idx)==std::forward<Type>(value)) return idx;
        }
        return -1;
    }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&& v1, Type&& v2) 的二元谓词函数，v1为element-value，v2为input-value
    int index(Type&& value,CompareFunction compareFunction) const {
        for (int idx = 0; idx <(int)StdList::size();++idx){
            if (compareFunction(this->operator[](idx),std::forward<Type>(value))) return idx;
        }
        return -1;
    }

    void insert(typename CList::size_type index, Type&& value)
    {
        typename StdList::iterator it=StdList::begin();
        while (index --)  ++it;
        StdList::insert(it, std::forward<Type>(value));
    }

    const Type& last() const{ return this->operator[](StdList::size() - 1); }

    void prepend(Type&& value) { StdList::emplace(StdList::begin(), std::forward<Type>(value)); }

    void push_back(Type&& value){ StdList::emplace_back(std::forward<Type>(value)); }
    void push_back(const CList& other){  StdList::insert(StdList::end(),other.begin(),other.end());}
    void push_back(const std::set<Type>& set){ StdList::insert(StdList::end(),set.begin(),set.end()); }
    void push_back(const std::vector<Type>& vec){ StdList::insert(StdList::end(),vec.begin(),vec.end()); }

    void push_front(Type&& value) { StdList::emplace(StdList::begin(), std::forward<Type>(value)); }

    void remove(Type&& value) { StdList::remove(std::forward<Type>(value)); }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&& v1, Type&& v2) 的二元谓词函数，v1为element-value，v2为input-value
    void remove(Type&& value, CompareFunction compareFunction)
    {
        StdList::remove_if(std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)));
    }
    void removeAll(){ StdList::clear(); }
    void removeAt(unsigned int index)
    {
        typename StdList::iterator it=StdList::begin();
        while (index --)  ++it;
        StdList::erase(it);
    }
    void removeFirst(){ StdList::pop_front(); }
    void removeLast(){ StdList::pop_back(); }
    void removeRange(unsigned int from, unsigned int to)
    {
        typename StdList::iterator first=StdList::begin(),last=StdList::begin();
        while (from --)  ++first;
        while (to --)  ++last;
        StdList::erase(first,last);
    }
    void replace(unsigned int index, Type&& value){ this->operator[](index) = std::forward<Type>(value); }

    void swap (std::list<Type,Alloc>& other){ return StdList::swap(other);}
    void swap (unsigned int index1,unsigned int index2){ return std::swap(this->operator[](index1),this->operator[](index2));}

    const Type& takeAt(unsigned int index) const{ return this->operator[](index); }
    const Type& takeFirst() const { return *StdList::begin(); }
    const Type& takeLast() const { return this->operator[](StdList::size() - 1); }

    std::list<Type> toStdList() const { return *this; }
    std::vector<Type> toStdVec() const{ return std::vector<Type>(StdList::begin(),StdList::end());}
    std::set<Type> toStdSet() const{ return std::set<Type>(StdList::begin(),StdList::end());}

    const Type& value(unsigned int index) const { return this->operator[](index); }
};
}
