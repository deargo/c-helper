#include "test.h"
#include <thread>
#include <chrono>
#include "library.hpp"
#include "command.hpp"
#include "threadpool.hpp"
#include "vector.hpp"
#include "list.hpp"
#include "linklist.hpp"

using namespace std;

#   ifdef WIN32
#       define __FUNC__ __FUNCTION__
#   else
#       define __FUNC__ __func__
#   endif

#define TEST_FUNC_begin cout << endl << "=============================begin " << __FUNC__ << endl
#define TEST_FUNC_ended cout << endl << "=============================ended " << __FUNC__ << endl <<endl

namespace CppHelperTest
{

void CLibraryTest()
{
    TEST_FUNC_begin;
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

    TEST_FUNC_ended;
}


struct STA
{
    int m_a;
    int operator()(){ return m_a; }
    int operator()(int n){ return m_a + n; }
    int triple0(){ return m_a * 3; }
    int triple(int a){ return m_a * 3 + a; }
    int triple1() const { return m_a * 3; }
    const int triple2(int a) const { return m_a * 3+a; }
    void triple3(){ cout << "" <<endl; }
};

int add_one(int n)
{
    return n + 1;
}
void CCommandTest()
{
    TEST_FUNC_begin;

    CppHelper::CCommand<int> cmd;
    //普通函数
    cmd.Wrap(add_one, 0);
    cout << cmd.Excecute()<< endl;

    //lambda表达式
    cmd.Wrap([](int n){return n + 1; }, 1);
    cout << cmd.Excecute()<< endl;

    //函数对象
    cmd.Wrap(STA());
    cout << cmd.Excecute()<< endl;
    cmd.Wrap(STA(), 4);
    cout << cmd.Excecute()<< endl;

    STA t = { 10 };
    int x = 3;
    //成员函数
    cmd.Wrap(&STA::triple0, &t);
    cout << cmd.Excecute()<< endl;
    cmd.Wrap(&STA::triple, &t, x);
    cout << cmd.Excecute()<< endl;
    cmd.Wrap(&STA::triple, &t, 3);
    cout << cmd.Excecute()<< endl;

    //常量成员
    CppHelper::CCommand<const int> cmd1;
    cmd1.Wrap(&STA::triple2, &t, 3);
    cout << cmd1.Excecute()<< endl;

    CppHelper::CCommand<> cmd2;
    cmd2.Wrap(&STA::triple3, &t);
    cmd2.Excecute();

    TEST_FUNC_ended;
}


void CThreadPoolTest()
{
    TEST_FUNC_begin;
    CppHelper::CThreadPool pool(2);

    std::thread thd1([&pool]
    {
        auto thdid = this_thread::get_id();
        for (int i = 0; i < 10; i++)
        {
            pool.addTask([](std::thread::id thdid, int value)
            {
                cout << "synchronous thread1[" << thdid << "] value[" << value << "], handle thread: " << this_thread::get_id() << endl;
            },thdid,i+100);
        }
    });
    std::thread thd2([&pool]
    {
        auto thdid = this_thread::get_id();
        for (int i = 0; i < 10; i++)
        {
            pool.addTask([&](std::thread::id thdid, int value)
            {
                cout << "synchronous thread2[" << thdid << "] value[" << value << "], handle thread: " << this_thread::get_id() << endl;
            },thdid,i+200);
        }
    });

    cout << "sleep seconds(2) ..." << endl << endl;
    this_thread::sleep_for(std::chrono::seconds(2));

    cout << endl << "input any words to stop: ";
    getchar();
    pool.stop();
    thd1.join();
    thd2.join();

    TEST_FUNC_ended;
}

void CVectorTest()
{
    TEST_FUNC_begin;

    typedef std::set<int> StdSet;
    typedef std::list<int> StdList;
    typedef std::vector<int> StdVec;
    typedef CppHelper::CVector<int> NewVec;


#define TEST_vec_bool(b) (b ? "true" : "false")
#define TEST_vec_exec(flage,code) cout << "line["<< __LINE__ << "]: "<< flage << code << endl
#define TEST_vec_show(flage,val) cout << "line["<< __LINE__ << "]: "<< flage ;\
    for(auto it = val.begin();it!=val.end();++it) cout << *it << " ";\
    if(val.empty()) cout << "empty";\
    cout << endl

    int arr[] = {1,2,3,4,5,6,7,8,9};
    StdSet stdSet{11,12,13,14,15,16,17,18,19};
    StdList stdList{21,22,23,24,25,26,27,28,29};
    StdVec stdVec1{31,32,33,34,35,36,37,38,39};
    StdVec stdVec2{41,42,43,44,45,46,47,48,49};
    NewVec newVec1;
    NewVec newVec2(10);
    NewVec newVec3(stdSet);
    NewVec newVec4(stdList);
    NewVec newVec5(stdVec1);
    NewVec newVec6(arr,arr+9);

    TEST_vec_show(" stdSet: ",stdSet);
    TEST_vec_show("stdList: ",stdList);
    TEST_vec_show("stdVec1: ",stdVec1);
    TEST_vec_show("newVec1: ",newVec1);
    TEST_vec_show("newVec2: ",newVec2);
    TEST_vec_show("newVec3: ",newVec3);
    TEST_vec_show("newVec4: ",newVec4);
    TEST_vec_show("newVec5: ",newVec5);
    TEST_vec_show("newVec6: ",newVec6);

    TEST_vec_exec("newVec3 == newVec3 ? : ",TEST_vec_bool(newVec3 == newVec4));
    TEST_vec_exec("newVec3 != newVec3 ? : ",TEST_vec_bool(newVec3 != newVec4));

    newVec1 = newVec3;
    newVec1 += newVec4;
    newVec1 << 100 << 200<< 300;
    TEST_vec_show("newVec1: ",newVec1);

    TEST_vec_exec("newVec1.size()    : ",newVec1.size());
    TEST_vec_exec("newVec1.capacity(): ",newVec1.capacity());
    newVec1.clear();
    TEST_vec_exec("newVec1.size()    : ",newVec1.size());
    TEST_vec_exec("newVec1.capacity(): ",newVec1.capacity());
    TEST_vec_show("newVec1: ",newVec1);

    newVec1.append(555);
    newVec1.append(newVec3);
    newVec1.append(stdSet);
    newVec1.append(stdList);
    TEST_vec_show("newVec1: ",newVec1);

    newVec1.clear();
    newVec1.resize(20);
    TEST_vec_show("newVec3      : ",newVec3);
    TEST_vec_show("newVec1      : ",newVec1);
    newVec1.copy(newVec1.begin(),newVec3.begin()+3,newVec3.end());
    TEST_vec_show("newVec1.copy1: ",newVec1);
    newVec1.copy(newVec1.begin(),newVec3.begin(),newVec3.end(),[](int v){ return v>15; });
    TEST_vec_show("newVec1.copy2: ",newVec1);
    newVec1.copy(newVec1.begin()+6,newVec3.begin(),3);
    TEST_vec_show("newVec1.copy3: ",newVec1);

    newVec1.contains(200,[](const int& v1,int v2){ return v1>v2;});
    TEST_vec_exec("newVec1.contains(555) : ",TEST_vec_bool(newVec1.contains(555)));
    TEST_vec_exec("newVec1.contains(>200): ",TEST_vec_bool(newVec1.contains(200,[](const int& v1,int v2){ return v1>v2;})));
    TEST_vec_exec("newVec1.count(11) : ",newVec1.count(11));
    TEST_vec_exec("newVec1.count(>28): ",newVec1.count(28,[](int v1, int v2){ return v1>v2;}));

    TEST_vec_show("newVec2       : ",newVec2);
    newVec2.fill(4,111,newVec2.begin());
    TEST_vec_show("newVec2.fill-1: ",newVec2);
    newVec2.fill(222,newVec2.begin()+6,newVec2.end());
    TEST_vec_show("newVec2.fill-2: ",newVec2);

    TEST_vec_show("newVec3                     : ",newVec3);
    TEST_vec_show("stdSet                      : ",stdSet);
    TEST_vec_exec("newVec3.equal(stdSet.begin) : ",TEST_vec_bool(newVec3.equal(stdSet.begin())));
    newVec2 = newVec3;
    TEST_vec_show("newVec2                : ",newVec2);
    TEST_vec_exec("newVec2.equal(newVec3) : ",TEST_vec_bool(newVec2.equal(newVec3)));

    TEST_vec_exec("newVec2.find(13)  : ",*(newVec1.find(13)));
    TEST_vec_exec("newVec2.find(<=13): ",*(newVec1.find(13,[](int vecVal,int findVal){ return vecVal<=findVal;})));

    cout << "newVec1.for_each: ";
    newVec1.for_each([](int v){ cout << v << " "; });
    cout << endl;

    TEST_vec_exec("newVec1.index(15)            : ",newVec1.index(15));
    TEST_vec_exec("newVec1.index(<100 && >=15)  : ",newVec1.index(15,[](int vecVal,int findVal){ return vecVal<100&& vecVal>=findVal;}));

    TEST_vec_show("newVec1        : ",newVec1);
    newVec1.insert(newVec1.size(),NewVec::value_type(666));
    TEST_vec_exec("newVec1.last() : ",newVec1.last());

    newVec1.prepend(333);
    TEST_vec_exec("newVec1.first(): ",newVec1.first());
    TEST_vec_show("newVec1        : ",newVec1);

    newVec1.clear();
    TEST_vec_show("newVec1  : ",newVec1);
    TEST_vec_show("newVec4  : ",newVec3);
    TEST_vec_show(" stdSet: ",stdSet);
    TEST_vec_show("stdList: ",stdList);
    newVec1.push_back(11);
    newVec1.push_back(newVec3);
    newVec1.push_back(stdSet);
    newVec1.push_back(stdList);
    newVec1.push_front(444);
    TEST_vec_show("newVec1  : ",newVec1);

    newVec1.remove(11);
    TEST_vec_show("newVec1 remove-1 : ",newVec1);
    newVec1.remove(15,[](int vecVal,int findVal){ return vecVal < findVal+2 && vecVal >= findVal-2;});
    TEST_vec_show("newVec1 remove-2 : ",newVec1);
    newVec1.removeRange(3,6);
    TEST_vec_show("newVec1 remove-3 : ",newVec1);
    newVec1.removeFirst();
    newVec1.removeLast();
    newVec1.removeAt(3);
    TEST_vec_show("newVec1 remove2-4 : ",newVec1);
    TEST_vec_exec("newVec1.size()     : ",newVec1.size());
    TEST_vec_exec("newVec1.capacity() : ",newVec1.capacity());
    newVec1.removeAll();
    TEST_vec_show("newVec1 removeAll  : ",newVec1);
    TEST_vec_exec("newVec1.size()     : ",newVec1.size());
    TEST_vec_exec("newVec1.capacity() : ",newVec1.capacity());

    newVec1 = newVec3;
    TEST_vec_show("newVec1=newVec3          : ",newVec1);
    newVec1.replace(3,333);
    TEST_vec_show("newVec1.replace(3,333)   : ",newVec1);
    newVec1.sort();
    TEST_vec_show("newVec1.sort()    : ",newVec1);
    newVec1.sort([](int a,int b){return a>b;});
    TEST_vec_show("newVec1.sort(a>b) : ",newVec1);

    TEST_vec_show("newVec3               : ",newVec3);
    newVec1.swap(newVec3);
    TEST_vec_show("newVec1.swap(newVec3) : ",newVec1);
    TEST_vec_show("newVec3               : ",newVec3);
    newVec1.swap(3,7);
    TEST_vec_show("newVec1.swap(3,7) : ",newVec1);

    TEST_vec_exec("newVec1.takeAt(3)   : ",newVec1.takeAt(3));
    TEST_vec_exec("newVec1.takeFirst() : ",newVec1.takeFirst());
    TEST_vec_exec("newVec1.takeLast()  : ",newVec1.takeLast());
    TEST_vec_exec("newVec1.value(7)    : ",newVec1.value(7));

    TEST_vec_show("newVec5       : ",newVec5);
    TEST_vec_show("stdSet        : ",stdSet);
    TEST_vec_show("stdList       : ",stdList);
    stdSet = newVec5.toStdSet();
    stdList = newVec5.toStdList();
    TEST_vec_show("stdSet        : ",stdSet);
    TEST_vec_show("stdList       : ",stdList);

    TEST_FUNC_ended;
}


void CListTest()
{
    TEST_FUNC_begin;

    typedef std::set<int> StdSet;
    typedef std::list<int> StdList;
    typedef std::vector<int> StdVec;
    typedef CppHelper::CList<int> NewList;


#define TEST_list_bool(b) (b ? "true" : "false")
#define TEST_list_exec(flage,code) cout << "line["<< __LINE__ << "]: "<< flage << code << endl
#define TEST_list_show(flage,val) cout << "line["<< __LINE__ << "]: "<< flage ;\
    for(auto it = val.begin();it!=val.end();++it) cout << *it << " ";\
    if(val.empty()) cout << "empty";\
    cout << endl

    int arr[] = {1,2,3,4,5,6,7,8,9};
    StdSet stdSet{11,12,13,14,15,16,17,18,19};
    StdList stdList{21,22,23,24,25,26,27,28,29};
    StdVec stdVec1{31,32,33,34,35,36,37,38,39};

    NewList newList1;
    NewList newList2(10);
    NewList newList3(stdSet);
    NewList newList4(stdList);
    NewList newList5(stdVec1);
    NewList newList6(arr,arr+9);
    TEST_list_show(" stdSet: ",stdSet);
    TEST_list_show("stdList: ",stdList);
    TEST_list_show("stdVec1: ",stdVec1);
    TEST_list_show("newList1: ",newList1);
    TEST_list_show("newList2: ",newList2);
    TEST_list_show("newList3: ",newList3);
    TEST_list_show("newList4: ",newList4);
    TEST_list_show("newList5: ",newList5);
    TEST_list_show("newList6: ",newList6);

    TEST_list_exec("newList3 == newList4 ? : ",TEST_list_bool(newList3 == newList4));
    TEST_list_exec("newList3 != newList4 ? : ",TEST_list_bool(newList3 != newList4));

    newList1 = newList3;
    newList1 += newList4;
    newList1 << 100 << 200<< 300;
    TEST_list_show("newList1: ",newList1);

    TEST_list_exec("newList1.size()    : ",newList1.size());
    newList1.clear();
    TEST_list_exec("newList1.size()    : ",newList1.size());
    TEST_list_show("newList1: ",newList1);

    newList1.append(555);
    newList1.append(newList3);
    newList1.append(stdSet);
    newList1.append(stdList);
    TEST_list_show("newList1: ",newList1);

    TEST_list_exec("newList1.at(11) : ",newList1.at(11));

    newList1.contains(200,[](const int& v1,int v2){ return v1>v2;});
    TEST_list_exec("newList1.contains(555) : ",TEST_list_bool(newList1.contains(555)));
    TEST_list_exec("newList1.contains(>200): ",TEST_list_bool(newList1.contains(200,[](const int& v1,int v2){ return v1>v2;})));
    TEST_list_exec("newList1.count(11) : ",newList1.count(11));
    TEST_list_exec("newList1.count(>28): ",newList1.count(28,[](int v1, int v2){ return v1>v2;}));

    newList1.clear();
    newList1.resize(20);
    TEST_list_show("newList3      : ",newList3);
    TEST_list_show("newList1      : ",newList1);
    newList1.copy(newList3);
    TEST_list_show("newList1.copy1: ",newList1);
    newList1.copy(3,111);
    TEST_list_show("newList1.copy2: ",newList1);
    newList1.copy(newList3.begin(),newList3.end());
    TEST_list_show("newList1.copy3: ",newList1);
    newList1.copy(newList3.begin(),newList3.end(),[](int v){ return v>15; });
    TEST_list_show("newList1.copy4: ",newList1);
    newList1.copy(newList3.begin(),7);
    TEST_list_show("newList1.copy5: ",newList1);

    TEST_list_exec("newList1.count(15) : ",newList1.count(15));
    TEST_list_exec("newList1.count(>15): ",newList1.count(15,[](int v1,int v2){ return v1>v2; }));

    TEST_vec_show("newList1                  : ",newList1);
    TEST_vec_show("newList3                  : ",newList3);
    TEST_vec_exec("newList1.equal1(newList3) : ",TEST_vec_bool(newList1.equal(newList3)));
    TEST_vec_exec("newList1.equal2(newList3) : ",TEST_vec_bool(newList1.equal(newList3.begin())));
    TEST_vec_exec("newList1.equal3(newList3) : ",TEST_vec_bool(newList1.equal(newList3.begin(),[](int v1,int v2){ return v1==v2; })));

    newList1.clear();
    newList1.resize(5);
    TEST_list_show("newList1        : ",newList1);
    newList1.fill(111,newList1.begin(),newList1.end());
    TEST_list_show("newList1.fill-1 : ",newList1);
    newList1.resize(10);
    newList1.fill(5,222,newList1.begin());
    TEST_list_show("newList1.fill-2 : ",newList1);

    newList1.copy(newList3);
    TEST_list_show("newList1           : ",newList1);
    TEST_list_exec("newList2.find(13)  : ",*(newList1.find(13)));
    TEST_list_exec("newList2.find(<=13): ",*(newList1.find(13,[](int vecVal,int findVal){ return vecVal<=findVal;})));

    cout << "newList1.for_each: ";
    newList1.for_each([](int v){ cout << v << " "; });
    cout << endl;

    TEST_list_exec("newList1.index(15)            : ",newList1.index(15));
    TEST_list_exec("newList1.index(<100 && >=15)  : ",newList1.index(15,[](int vecVal,int findVal){ return vecVal<100&& vecVal>=findVal;}));

    TEST_list_show("newList1              : ",newList1);
    newList1.insert(3,333);
    TEST_list_show("newList1.insert(3,333): ",newList1);

    TEST_list_show("newList1        : ",newList1);
    newList1.insert(newList1.size(),NewList::value_type(666));
    TEST_list_exec("newList1.last() : ",newList1.last());

    newList1.prepend(333);
    TEST_list_exec("newList1.first(): ",newList1.first());
    TEST_list_show("newList1        : ",newList1);

    newList1.clear();
    TEST_list_show("newList1  : ",newList1);
    TEST_list_show("newList3  : ",newList3);
    TEST_list_show(" stdSet: ",stdSet);
    TEST_list_show("stdList: ",stdList);
    newList1.push_back(11);
    newList1.push_back(newList3);
    newList1.push_back(stdSet);
    newList1.push_back(stdList);
    newList1.push_front(444);
    TEST_list_show("newList1  : ",newList1);

    newList1.remove(11);
    TEST_list_show("newList1 remove-1 : ",newList1);
    newList1.remove(15,[](int vecVal,int findVal){ return vecVal < findVal+2 && vecVal >= findVal-2;});
    TEST_list_show("newList1 remove-2 : ",newList1);
    newList1.removeRange(3,6);
    TEST_list_show("newList1 remove-3 : ",newList1);
    newList1.removeFirst();
    newList1.removeLast();
    newList1.removeAt(3);
    TEST_list_show("newList1 remove-4 : ",newList1);
    TEST_list_exec("newList1.size()     : ",newList1.size());
    newList1.removeAll();
    TEST_list_show("newList1 removeAll  : ",newList1);
    TEST_list_exec("newList1.size()     : ",newList1.size());

    newList1 = newList3;
    TEST_list_show("newList1=newList3      : ",newList1);
    newList1.replace(3,333);
    TEST_list_show("newList1.replace(3,333): ",newList1);

    TEST_list_show("newList3               : ",newList3);
    newList1.swap(newList3);
    TEST_list_show("newList1.swap(newList3) : ",newList1);
    TEST_list_show("newList3               : ",newList3);
    newList1.swap(3,7);
    TEST_list_show("newList1.swap(3,7) : ",newList1);

    TEST_list_exec("newList1.takeAt(3)   : ",newList1.takeAt(3));
    TEST_list_exec("newList1.takeFirst() : ",newList1.takeFirst());
    TEST_list_exec("newList1.takeLast()  : ",newList1.takeLast());
    TEST_list_exec("newList1.value(7)    : ",newList1.value(7));


    TEST_list_show("newList5      : ",newList5);
    TEST_list_show("stdSet        : ",stdSet);
    TEST_list_show("stdList       : ",stdList);
    TEST_list_show("stdVec1       : ",stdVec1);
    stdSet = newList5.toStdSet();
    stdVec1 = newList5.toStdVec();
    stdList = newList5.toStdList();
    TEST_list_show("stdSet        : ",stdSet);
    TEST_list_show("stdList       : ",stdList);
    TEST_list_show("stdVec1       : ",stdVec1);

    TEST_FUNC_ended;
}


void CLinkListTest()
{
    TEST_FUNC_begin;

    typedef std::set<int> StdSet;
    typedef std::list<int> StdList;
    typedef std::vector<int> StdVec;
    typedef CppHelper::CLinkList<int> LinkList;

#define TEST_link_bool(b) (b ? "true" : "false")
#define TEST_link_exec(flage,code) cout << "line["<< __LINE__ << "]: "<< flage << code << endl
#define TEST_link_show(flage,val) cout << "line["<< __LINE__ << "]: "<< flage ;\
    for(auto it = val.begin();it!=val.end();++it) cout << *it << " ";\
    if(val.empty()) cout << "empty";\
    cout << endl

    int arr[] = {1,2,3,4,5,6,7,8,9};
    StdSet stdSet{11,12,13,14,15,16,17,18,19};
    StdList stdList{21,22,23,24,25,26,27,28,29};
    StdVec stdVec{31,32,33,34,35,36,37,38,39};

    LinkList linkList1;
    LinkList linkList2(10);
    LinkList linkList3(stdSet);
    LinkList linkList4(stdList);
    LinkList linkList5(stdVec);
    LinkList linkList6(arr,arr+9);

    TEST_link_show(" stdSet  : ",stdSet);
    TEST_link_show("stdList  : ",stdList);
    TEST_link_show("stdVec   : ",stdVec);
    TEST_link_show("linkList1: ",linkList1);
    TEST_link_show("linkList2: ",linkList2);
    TEST_link_show("linkList3: ",linkList3);
    TEST_link_show("linkList4: ",linkList4);
    TEST_link_show("linkList5: ",linkList5);
    TEST_link_show("linkList6: ",linkList6);

    TEST_link_exec("linkList3 == linkList4 ? : ",TEST_link_bool(linkList3 == linkList4));
    TEST_link_exec("linkList3 != linkList4 ? : ",TEST_link_bool(linkList3 != linkList4));

    linkList1 = linkList3;
    linkList1 += linkList4;
    linkList1 << 100 << 200<< 300;
    TEST_link_show("linkList1: ",linkList1);

    TEST_link_exec("linkList1.size()    : ",linkList1.size());
    linkList1.clear();
    TEST_link_exec("linkList1.size()    : ",linkList1.size());
    TEST_link_show("linkList1: ",linkList1);

    linkList1.append(555);
    linkList1.append(linkList3);
    linkList1.append(stdSet);
    linkList1.append(stdList);
    linkList1.append(stdVec);
    TEST_link_show("linkList1: ",linkList1);

    linkList1.clear();
    linkList1 << 3;
    linkList1.assign(5,555);
    TEST_link_show("linkList1.assign-1 : ",linkList1);
    TEST_link_show("stdSet             : ",stdSet);
    linkList1.assign(stdSet.begin(),stdSet.end());
    TEST_link_show("linkList1.assign-2 : ",linkList1);

    //TEST_link_exec("linkList1.at(9)   : ",linkList1.at(9)); //error
    TEST_link_exec("linkList1.at(7) : ",linkList1.at(7));
    TEST_link_exec("linkList1.back  : ",linkList1.back());
    TEST_link_exec("linkList1.begin : ",*(linkList1.begin()));
    //TEST_link_exec("linkList1.end() : ",*(linkList1.end())); //error

    linkList1<< 100<< 200<<300;
    linkList1.contains(200,[](const int& v1,int v2){ return v1>v2;});
    TEST_link_exec("linkList1.contains(555) : ",TEST_link_bool(linkList1.contains(555)));
    TEST_link_exec("linkList1.contains(>200): ",TEST_link_bool(linkList1.contains(200,[](const int& v1,int v2){ return v1>v2;})));
    TEST_link_exec("linkList1.count(11) : ",linkList1.count(11));
    TEST_link_exec("linkList1.count(>28): ",linkList1.count(28,[](int v1, int v2){ return v1>v2;}));

    TEST_link_exec("linkList1.empty() : ",TEST_link_bool(linkList1.empty()));
    linkList1.erase(linkList1.begin());
    TEST_link_show("linkList1.erase-1 : ",linkList1);
    linkList1.erase(linkList1.begin()+5,linkList1.end());
    TEST_link_show("linkList1.erase-2 : ",linkList1);
    linkList1.clear();
    TEST_link_exec("linkList1.empty() : ",TEST_link_bool(linkList1.empty()));
    linkList1.resize(20);
    TEST_link_exec("linkList1.end()   : ",*(linkList1.begin()));

    linkList1 = linkList3;
    TEST_link_show("linkList1           : ",linkList1);
    TEST_link_exec("linkList2.find(13)  : ",*(linkList1.find(13)));
    linkList1[3]=10;
    TEST_link_exec("linkList2.find(<=13): ",*(linkList1.find(13,[](int vecVal,int findVal){ return vecVal<=findVal;})));

    cout << "linkList1.for_each: ";
    linkList1.for_each([](int v){ cout << v << " "; });
    cout << endl;

    TEST_link_exec("linkList1.front: ",linkList1.front());
    linkList1 << 33;
    TEST_link_exec("linkList1.index(15)           : ",linkList1.index(15));
    TEST_link_exec("linkList1.index(<100 && >=15) : ",linkList1.index(15,[](int vecVal,int findVal){ return vecVal<100&& vecVal>=findVal;}));

    TEST_link_show("linkList1                    : ",linkList1);
    linkList1.insert(linkList1.begin(),333);
    TEST_link_show("linkList1.insert-1: ",linkList1);
    linkList1.insert(linkList1.begin()+3,LinkList::size_type(3),LinkList::value_type(666));
    TEST_link_show("linkList1.insert-2: ",linkList1);
    linkList1.insert(linkList1.end(),linkList3.begin()+7,linkList3.end());
    TEST_link_show("linkList1.insert-3: ",linkList1);

    TEST_link_exec("linkList1.size()    : ",linkList1.size());
    TEST_link_exec("linkList1.max_size(): ",linkList1.max_size());
    linkList1.clear();
    TEST_link_exec("linkList1.size()    : ",linkList1.size());

    linkList1 = linkList3;
    TEST_link_show("linkList1         : ",linkList1);
    TEST_link_show("linkList4         : ",linkList4);
    TEST_link_show("linkList5         : ",linkList5);
    linkList1.merge(linkList4);
    TEST_link_show("linkList1.merge-1 : ",linkList1);
    linkList1.merge(linkList5,[](int v1,int v2){ return v1>v2;});
    TEST_link_show("linkList1.merge-2 : ",linkList1);
    TEST_link_show("linkList5         : ",linkList5);

    linkList1.pop_back();
    TEST_link_show("linkList1.pop_back  : ",linkList1);
    linkList1.pop_front();
    TEST_link_show("linkList1.pop_front : ",linkList1);

    linkList1.clear();
    TEST_link_show("linkList1  : ",linkList1);
    TEST_link_show("linkList3  : ",linkList3);
    TEST_link_show(" stdSet    : ",stdSet);
    TEST_link_show("stdList    : ",stdList);
    linkList1.push_back(11);
    linkList1.push_back(linkList3);
    linkList1.push_back(stdSet);
    linkList1.push_back(stdList);
    linkList1.push_back(stdVec);
    TEST_link_show("linkList1.push_back: ",linkList1);

    linkList1.clear();
    TEST_link_show("linkList1  : ",linkList1);
    linkList1.push_front(11);
    linkList1.push_front(linkList3);
    linkList1.push_front(stdSet);
    linkList1.push_front(stdList);
    linkList1.push_front(stdVec);
    TEST_link_show("linkList1.push_front: ",linkList1);

    TEST_link_exec("linkList1.rbegin(): ",*linkList1.rbegin());
    //TEST_link_exec("linkList1.rend()  : ",*linkList1.rend()); //error

    linkList1.remove(11);
    TEST_link_show("linkList1 remove   : ",linkList1);
    linkList1.remove_if([](int v){ return v>15 && v<18;});
    TEST_link_show("linkList1 remove_if: ",linkList1);
    linkList1.removeAll();
    TEST_link_show("linkList1 removeAll: ",linkList1);

    linkList1.resize(20);
    TEST_link_show("linkList1.resize-1: ",linkList1);
    linkList1 = linkList3;
    TEST_link_show("linkList1.resize-2: ",linkList1);
    linkList1.resize(7);
    TEST_link_show("linkList1.resize-7: ",linkList1);
    linkList1.reverse();
    TEST_link_show("linkList1.reverse: ",linkList1);

    linkList1.sort();
    TEST_link_show("linkList1.sort-1 : ",linkList1);
    linkList1.push_front(222);
    linkList1.sort([](int v1,int v2){return v1<v2;});
    TEST_link_show("linkList1.sort-2 : ",linkList1);

    auto it = linkList1.begin()+1;
    TEST_link_show("linkList3     : ",linkList3);
    linkList1.splice(it,linkList3);
    TEST_link_show("linkList1.splice     : ",linkList1);
    TEST_link_show("linkList3     : ",linkList3);
    linkList3.splice(linkList3.begin(),linkList1,linkList3.begin()+1);

    TEST_link_show("linkList3.splice     : ",linkList3);
    TEST_link_show("linkList1     : ",linkList1);
    linkList1.splice ( linkList1.begin(), linkList1, linkList1.begin()+3, linkList1.end());
    TEST_link_show("linkList1     : ",linkList1);

    linkList1.swap(linkList3);
    TEST_link_show("linkList1.swap(linkList3) : ",linkList1);
    TEST_link_show("linkList3                 : ",linkList3);

    TEST_link_show("stdSet        : ",stdSet);
    TEST_link_show("stdList       : ",stdList);
    TEST_link_show("stdVec        : ",stdVec);
    stdSet = linkList3.toStdSet();
    stdVec = linkList3.toStdVec();
    stdList = linkList3.toStdList();
    TEST_link_show("stdSet        : ",stdSet);
    TEST_link_show("stdList       : ",stdList);
    TEST_link_show("stdVec        : ",stdVec);

    linkList1 = linkList3;
    TEST_link_show("linkList1           : ",linkList1);
    linkList1.unique();
    TEST_link_show("linkList1.unique    : ",linkList1);
    linkList1 = linkList3;
    TEST_link_show("linkList1           : ",linkList1);
    linkList1.uniqueAll();
    TEST_link_show("linkList1.uniqueAll : ",linkList1);

    TEST_FUNC_ended;
}

}
