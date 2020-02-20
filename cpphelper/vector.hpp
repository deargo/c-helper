#pragma once

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <functional>

namespace CppHelper
{
template<class Type , class Alloc = std::allocator<Type> >
class CVector : public std::vector<Type,Alloc>{
    using StdVec = std::vector<Type,Alloc>;

public:
    explicit CVector(const Alloc& alloc = Alloc()):StdVec(alloc) {}                                          //默认构造函数
    explicit CVector(size_t n, const Type& val = Type(), const Alloc& alloc = Alloc()):StdVec(n,val,alloc){} //容量构造函数
    CVector(CVector&& other):StdVec(std::forward<CVector>(other)){ }                                                           //拷贝构造函数
    CVector(const std::vector<Type,Alloc>& vec){ StdVec::assign(vec.begin(), vec.end()); }                   //可用std::vector初始化
    template <class InitIterator>                                                                            //可用迭代器初始化
    CVector (InitIterator first, InitIterator last, const Alloc& alloc = Alloc()):StdVec(first,last,alloc){}
    explicit CVector(const std::set<Type>& set){ StdVec::assign(set.begin(), set.end()); }                   //可用std::set初始化
    explicit CVector(const std::list<Type>& list){ StdVec::assign(list.begin(), list.end()); }               //可用std::list初始化

public:
    operator std::vector<Type,Alloc>(){ return *this; }                                                  //类型转换函数，可直接转换为std::vector
    bool operator == (const CVector& other) const{ return std::equal(StdVec::begin(),StdVec::end(),other.begin()); }
    bool operator !=(const CVector& other) const { return false == std::equal(StdVec::begin(),StdVec::end(),other.begin()); }
    CVector& operator =(const CVector& other){StdVec::assign(other.begin(), other.end()); return *this;} //赋值构造函数
    CVector& operator +(const CVector& other){ push_back(other); return *this; }
    CVector& operator +=(const CVector& other){ push_back(other); return *this; }
    CVector& operator<<(Type&& value){ push_back(std::forward<Type>(value)); return *this; }
    CVector& operator<<(const CVector& other){ push_back(other); return *this; }
    CVector& operator<<(const std::set<Type>& set){ push_back(set); return *this; }
    CVector& operator<<(const std::list<Type>& list){ push_back(list); return *this; }

public:
    void append(Type&& value){ push_back(std::forward<Type>(value)); }
    void append(const CVector& other){ push_back(other); }
    void append(const std::set<Type>& set){ push_back(set); }
    void append(const std::list<Type>& list){ push_back(list); }

    bool contains(Type&& value) const
    {
        return  std::find(StdVec::begin(), StdVec::end(),std::forward<Type>(value)) != StdVec::end();
    }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&&, Type&&) 的二元谓词函数
    bool contains(Type&& value,CompareFunction compareFunction) const
    {
        return  std::find_if(StdVec::begin(), StdVec::end(),
                             std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)))
                != StdVec::end();
    }

    template <class CopyIterator>
    typename CVector::iterator copy(typename CVector::iterator thisDesc, CopyIterator first,CopyIterator last)
    {
        return std::copy(first,last,thisDesc);
    }
    template <class CopyIterator, class Function>
    typename CVector::iterator copy(typename CVector::iterator thisDesc, CopyIterator first,CopyIterator last, Function func)
    {
        return std::copy_if(first,last,thisDesc, func);
    }
    template <class CopyIterator>
    typename CVector::iterator copy(typename CVector::iterator thisDesc, CopyIterator first,typename CVector::size_type n)
    {
        return std::copy_n(first,n,thisDesc);
    }

    int count(Type&& value) const
    {
        return std::count(StdVec::begin(), StdVec::end(),std::forward<Type>(value));
    }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&&, Type&&) 的二元谓词函数
    int count(Type&& value,CompareFunction compareFunction) const
    {
        return  std::count_if(StdVec::begin(), StdVec::end(),
                              std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)));
    }

    bool equal(const CVector& other) const
    {
        return std::equal(StdVec::begin(),StdVec::end(),other.begin());
    }
    template <class CompareIterator>
    bool equal(CompareIterator compareIterator) const
    {
        return std::equal(StdVec::begin(),StdVec::end(),compareIterator);
    }
    template <class CompareIterator, class CompareFunction> //需要传入类似 bool (*compareFunction)(Type&&, Type&&) 的二元谓词函数
    bool equal(CompareIterator compareIterator,CompareFunction compareFunction) const
    {
        return std::equal(StdVec::begin(),StdVec::end(),compareIterator,compareFunction);
    }

    typename CVector::iterator find(Type&& value)
    {
        return  std::find(StdVec::begin(), StdVec::end(),std::forward<Type>(value));
    }
    typename CVector::const_iterator find(Type&& value) const
    {
        return  std::find(StdVec::begin(), StdVec::end(),std::forward<Type>(value));
    }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&&, Type&&) 的二元谓词函数
    typename CVector::iterator find(Type&& value,CompareFunction compareFunction)
    {
        return  std::find_if(StdVec::begin(), StdVec::end(),
                             std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)));
    }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&&, Type&&) 的二元谓词函数
    typename CVector::const_iterator find(Type&& value,CompareFunction compareFunction ) const
    {
        return  std::find_if(StdVec::begin(), StdVec::end(),
                             std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value)));
    }

    void fill(typename CVector::iterator first, typename CVector::iterator last, Type&& value) { std::fill(first,last,std::forward<Type>(value)); }
    void fill(typename CVector::iterator first, typename CVector::size_type n, Type&& value)  { std::fill_n(first,n,std::forward<Type>(value)); }

    Type first() const{ if (StdVec::empty()) return Type(); return *StdVec::begin(); }

    template <class Function>
    Function for_each(Function func){ return std::for_each(StdVec::begin(), StdVec::end(),func); }

    int index(Type&& value) const {
        for (int idx = 0; idx <(int)StdVec::size();++idx){
            if (StdVec::operator[](idx)==std::forward<Type>(value)) return idx;
        }
        return -1;
    }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&&, Type&&) 的二元谓词函数
    int index(Type&& value,CompareFunction compareFunction) const {
        for (int idx = 0; idx <(int)StdVec::size();++idx){
            if (compareFunction(StdVec::operator[](idx),std::forward<Type>(value))) return idx;
        }
        return -1;
    }

    void insert(unsigned int index, Type&& value){ StdVec::emplace(StdVec::begin() + index, std::forward<Type>(value)); }

    Type last() const{ if (StdVec::empty()) return Type(); return StdVec::operator[](StdVec::size() - 1); }

    void pop_front(){ if (!StdVec::empty()) StdVec::erase(StdVec::begin()); }

    void prepend(Type&& value) { StdVec::emplace(StdVec::begin(), std::forward<Type>(value)); }

    void push_back(Type&& value){ StdVec::emplace_back(std::forward<Type>(value)); }
    void push_back(const CVector& other){  StdVec::insert(StdVec::end(),other.begin(),other.end());}
    void push_back(const std::set<Type>& set){ StdVec::insert(StdVec::end(),set.begin(),set.end()); }
    void push_back(const std::list<Type>& list){ StdVec::insert(StdVec::end(),list.begin(),list.end()); }

    void push_front(Type&& value) { StdVec::emplace(StdVec::begin(), std::forward<Type>(value)); }

    void remove(Type&& value)
    {
        StdVec::erase(std::remove(std::begin(*this), std::end(*this),std::forward<Type>(value)), std::end(*this));
    }
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&&, Type&&) 的二元谓词函数
    void remove(Type&& value, CompareFunction compareFunction)
    {
        StdVec::erase(std::remove_if(std::begin(*this), std::end(*this),
                                     std::bind(compareFunction,std::placeholders::_1,std::forward<Type>(value))),
                      std::end(*this));}
    void removeAll(){ StdVec::clear(); StdVec::shrink_to_fit();}
    void removeAt(unsigned int index){ StdVec::erase(StdVec::begin() + index); }
    void removeFirst(){ if (!StdVec::empty()) {StdVec::erase(StdVec::begin());} }
    void removeLast(){ if (!StdVec::empty()) {StdVec::pop_back();} }
    void removeRange(unsigned int from, unsigned int to)
    {
        while(to-from)
        {
            StdVec::erase(StdVec::begin()+from);
            to--;
        }
    }

    void replace(unsigned int pos, Type&& value){ StdVec::operator[](pos) = std::forward<Type>(value); }

    void sort(){return std::sort(StdVec::begin(),StdVec::end());}
    template <class CompareFunction>  //需要传入类似 bool (*compareFunction)(Type&&, Type&&) 的二元谓词函数
    void sort(CompareFunction compareFunction){return std::sort(StdVec::begin(),StdVec::end(),compareFunction);}

    void swap (std::vector<Type,Alloc>& other){ return StdVec::swap(other);}
    void swap (unsigned int pos1,unsigned int pos2){ return std::swap(StdVec::operator[](pos1),StdVec::operator[](pos2));}

    Type takeAt(unsigned int pos) const{ return StdVec::operator[](pos); }
    Type takeFirst() const { if (StdVec::empty()) return Type(); return *StdVec::begin(); }
    Type takeLast() const { if (StdVec::empty()) return Type(); return StdVec::operator[](StdVec::size() - 1); }

    std::list<Type> toStdList() const{ return std::list<Type>(StdVec::begin(),StdVec::end());}
    std::set<Type> toStdSet() const{ return std::set<Type>(StdVec::begin(),StdVec::end());}

    Type value(unsigned int pos) const { return StdVec::operator[](pos); }
};

}
