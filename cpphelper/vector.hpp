#pragma once

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <vector>
#include <set>
#include <list>
#include <algorithm>

namespace CppHelper
{
template<typename Type , class Alloc = std::allocator<Type> >
class CVector : public std::vector<Type,Alloc>{
    using StdVec = std::vector<Type,Alloc>;
public:
    explicit CVector(const Alloc& alloc = Alloc()):StdVec(alloc) {}                                           //默认构造函数
    explicit CVector(size_t n, const Type& val = Type(), const Alloc& alloc = Alloc()):StdVec(n,val,alloc){}  //容量构造函数
    CVector(const CVector& ohter):StdVec(ohter){ }                                                            //拷贝构造函数
    CVector(const std::vector<Type,Alloc>& vec){ StdVec::assign(vec.begin(), vec.end()); }                    //可用std::vector初始化
    template <class InputIterator>                                                                            //可用迭代器初始化
    CVector (InputIterator first, InputIterator last, const Alloc& alloc = Alloc()):StdVec(first,last,alloc){}
    explicit CVector(const std::set<Type>& set){ StdVec::assign(set.begin(), set.end()); }                    //可用std::set初始化
    explicit CVector(const std::list<Type>& list){ StdVec::assign(list.begin(), list.end()); }                //可用std::list初始化

public:
    operator std::vector<Type,Alloc>(){ return *this; }                                                  //类型转换函数，可直接转换为std::vector
    CVector& operator= (const CVector& other){StdVec::assign(other.begin(), other.end()); return *this;} //赋值构造函数
    CVector& operator +(const CVector& other){ push_back(other); return *this; }
    CVector& operator +=(const CVector& other){ push_back(other); return *this; }
    CVector& operator<<(const Type& value){ push_back(value); return *this; }
    CVector& operator<<(const CVector& other){ push_back(other); return *this; }
    CVector& operator<<(const std::set<Type>& set){ push_back(set); return *this; }
    CVector& operator<<(const std::list<Type>& list){ push_back(list); return *this; }

public:
    inline void append(const Type& value){ push_back(value); }
    inline void append(const CVector& other){ push_back(other); }
    inline void append(const std::set<Type>& set){ push_back(set); }
    inline void append(const std::list<Type>& list){ push_back(list); }
    inline bool contains(const Type& value) const {  return  std::find(StdVec::begin(), StdVec::end(),value) != StdVec::end();}
    inline bool endWith(const Type& value){ if (StdVec::empty()) return false; return StdVec::operator[](StdVec::size() - 1) == value; }
    inline Type first() const{ if (StdVec::empty()) return Type(); return *StdVec::begin(); }
    int indexof(const Type& value) const {
        int index = 0;
        for (index = 0; index <(int)StdVec::size();++index){
            if (StdVec::operator[](index)==value) return index;
        }
        return -1;
    }
    inline void insert(unsigned int index, const Type& value){ StdVec::emplace(StdVec::begin() + index, value); }
    Type last() const{ if (StdVec::empty()) return Type(); return StdVec::operator[](StdVec::size() - 1); }
    int lastIndexof(const Type& value) const {
        int index = indexof(value);
        return index >= 0 ? (StdVec::size() - index - 1) : -1;
    }
    inline void pop_front(){ if (!StdVec::empty()) StdVec::erase(StdVec::begin()); }
    inline void prepend(const Type& value) { StdVec::emplace(StdVec::begin(), value); }
    inline void push_back(const Type& value){ StdVec::emplace_back(value); }
    inline void push_back(const CVector& other){  StdVec::insert(StdVec::end(),other.begin(),other.end());}
    inline void push_back(const std::set<Type>& set){ StdVec::insert(StdVec::end(),set.begin(),set.end()); }
    inline void push_back(const std::list<Type>& list){ StdVec::insert(StdVec::end(),list.begin(),list.end()); }
    inline void push_front(const Type& value) { StdVec::emplace(StdVec::begin(), value); }
    inline void remove(const Type &value) { StdVec::erase(std::remove(std::begin(*this), std::end(*this),value), std::end(*this));}
    inline void remove(unsigned int from, unsigned int to){ while(to-from) { StdVec::erase(StdVec::begin()+from); to--;} }
    inline void removeAll(){ StdVec::clear(); StdVec::shrink_to_fit();}
    inline void removeAt(unsigned int index){ StdVec::erase(StdVec::begin() + index); }
    inline void removeFirst(){ if (!StdVec::empty()) {StdVec::erase(StdVec::begin());} }
    inline void removeLast(){ if (!StdVec::empty()) {StdVec::pop_back();} }
    inline void replace(unsigned int pos, const Type &value){ StdVec::operator[](pos) = value; }
    inline bool startWith(const Type& value) const{ if (StdVec::empty()) return false; return *StdVec::begin() == value; }
    inline void swap (std::vector<Type,Alloc>& other){ return StdVec::swap(other);}
    inline void swap (unsigned int pos1,unsigned int pos2){ return std::swap(StdVec::operator[](pos1),StdVec::operator[](pos2));}
    inline Type takeAt(unsigned int pos) const{ return StdVec::operator[](pos); }
    inline Type takeFirst() const { if (StdVec::StdVec::empty()) return Type(); return *StdVec::begin(); }
    inline Type takeLast() const { if (StdVec::StdVec::empty()) return Type(); return StdVec::operator[](StdVec::size() - 1); }
    std::list<Type> toList() const{ return std::list<Type>(StdVec::begin(),StdVec::end());}
    std::set<Type> toSet() const{ return std::set<Type>(StdVec::begin(),StdVec::end());}
    inline Type value(unsigned int pos) const { return StdVec::operator[](pos); }
};

}
