#include "test.h"
#include <thread>
#include "library.hpp"
#include "command.hpp"
#include "threadpool.hpp"
#include "vector.hpp"

using namespace std;

#   ifdef WIN32
#       define __FUNC__ __FUNCTION__
#   else
#       define __FUNC__ __func__
#   endif

#define TEST_FUNC_BEGIN cout << endl << "=============================begin " << __FUNC__ << endl
#define TEST_FUNC_ENDED cout << endl << "=============================ended " << __FUNC__ << endl <<endl

namespace CppHelperTest
{

void CLibraryTest()
{
    TEST_FUNC_BEGIN;
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

    TEST_FUNC_ENDED;
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
    TEST_FUNC_BEGIN;

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

    TEST_FUNC_ENDED;
}


void CThreadPoolTest()
{
    TEST_FUNC_BEGIN;
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

    TEST_FUNC_ENDED;
}

void CVectorTest()
{
    TEST_FUNC_BEGIN;

    typedef std::set<int> StdSet;
    typedef std::list<int> StdList;
    typedef std::vector<int> StdVec;
    typedef CppHelper::CVector<int> NewVec;


#define TEST_vec_bool(b) (b ? "true" : "false")
#define TEST_vec_exec(flage,code) cout << flage << code << endl
#define TEST_vec_print(flage,val) cout << flage ;\
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

    TEST_vec_print(" stdSet: ",stdSet);
    TEST_vec_print("stdList: ",stdList);
    TEST_vec_print("stdVec1: ",stdVec1);
    TEST_vec_print("stdVec2: ",stdVec2);
    TEST_vec_print("newVec1: ",newVec1);
    TEST_vec_print("newVec2: ",newVec2);
    TEST_vec_print("newVec3: ",newVec3);
    TEST_vec_print("newVec4: ",newVec4);
    TEST_vec_print("newVec5: ",newVec5);
    TEST_vec_print("newVec6: ",newVec6);

    TEST_vec_exec ("newVec3 == newVec3 ? : ",TEST_vec_bool(newVec3 == newVec4));
    TEST_vec_exec ("newVec3 != newVec3 ? : ",TEST_vec_bool(newVec3 != newVec4));

    newVec1 = newVec3;
    newVec1 += newVec4;
    newVec1 << 100 << 200<< 300;
    TEST_vec_print("newVec1: ",newVec1);

    TEST_vec_exec ("newVec1.size()    : ",newVec1.size());
    TEST_vec_exec ("newVec1.capacity(): ",newVec1.capacity());
    newVec1.clear();
    TEST_vec_exec ("newVec1.size()    : ",newVec1.size());
    TEST_vec_exec ("newVec1.capacity(): ",newVec1.capacity());
    TEST_vec_print("newVec1: ",newVec1);

    newVec1.append(555);
    newVec1.append(newVec3);
    newVec1.append(stdSet);
    newVec1.append(stdList);
    TEST_vec_print("newVec1: ",newVec1);

    newVec1.clear();
    newVec1.resize(20);
    TEST_vec_print("newVec3      : ",newVec3);
    TEST_vec_print("newVec1      : ",newVec1);
    newVec1.copy(newVec1.begin(),newVec3.begin()+3,newVec3.end());
    TEST_vec_print("newVec1.copy1: ",newVec1);
    newVec1.copy(newVec1.begin(),newVec3.begin(),newVec3.end(),[](int v){ return v>15; });
    TEST_vec_print("newVec1.copy2: ",newVec1);
    newVec1.copy(newVec1.begin()+6,newVec3.begin(),3);
    TEST_vec_print("newVec1.copy3: ",newVec1);

    newVec1.contains(200,[](const int& v1,int v2){ return v1>v2;});
    TEST_vec_exec ("newVec1.contains(555) : ",TEST_vec_bool(newVec1.contains(555)));
    TEST_vec_exec ("newVec1.contains(>200): ",TEST_vec_bool(newVec1.contains(200,[](const int& v1,int v2){ return v1>v2;})));
    TEST_vec_exec ("newVec1.count(11) : ",newVec1.count(11));
    TEST_vec_exec ("newVec1.count(>28): ",newVec1.count(28,[](int v1, int v2){ return v1>v2;}));

    TEST_vec_print("newVec2                                         : ",newVec2);
    newVec2.fill(newVec2.begin(),4,111);
    TEST_vec_print("newVec2.fill(newVec2.begin(),4,111)             : ",newVec2);
    newVec2.fill(newVec2.begin()+6,newVec2.end(),222);
    TEST_vec_print("newVec2fill(newVec2.begin()+6,newVec2.end(),222): ",newVec2);

    TEST_vec_print("newVec3                     : ",newVec3);
    TEST_vec_print("stdSet                      : ",stdSet);
    TEST_vec_exec ("newVec3.equal(stdSet.begin) : ",TEST_vec_bool(newVec3.equal(stdSet.begin())));
    newVec2 = newVec3;
    TEST_vec_print("newVec2                : ",newVec2);
    TEST_vec_exec ("newVec2.equal(newVec3) : ",TEST_vec_bool(newVec2.equal(newVec3)));

    TEST_vec_exec("newVec2.find(13)  : ",*(newVec1.find(13)));
    TEST_vec_exec("newVec2.find(<=13): ",*(newVec1.find(13,[](int vecVal,int findVal){ return vecVal<=findVal;})));

    cout << "newVec1.for_each: ";
    newVec1.for_each([](int v){ cout << v << " "; });
    cout << endl;

    TEST_vec_exec("newVec1.index(15)            : ",newVec1.index(15));
    TEST_vec_exec("newVec1.index(<100 && >=15)  : ",newVec1.index(15,[](int vecVal,int findVal){ return vecVal<100&& vecVal>=findVal;}));

    TEST_vec_print("newVec1        : ",newVec1);
    newVec1.insert(newVec1.size(),666);
    TEST_vec_exec("newVec1.last() : ",newVec1.last());

    newVec1.prepend(333);
    TEST_vec_exec("newVec1.first(): ",newVec1.first());
    TEST_vec_print("newVec1        : ",newVec1);

    newVec1.clear();
    TEST_vec_print("newVec1  : ",newVec1);
    TEST_vec_print("newVec4  : ",newVec3);
    TEST_vec_print(" stdSet: ",stdSet);
    TEST_vec_print("stdList: ",stdList);
    newVec1.push_back(11);
    newVec1.push_back(newVec3);
    newVec1.push_back(stdSet);
    newVec1.push_back(stdList);
    newVec1.push_front(444);
    TEST_vec_print("newVec1  : ",newVec1);

    newVec1.remove(11);
    newVec1.remove(15,[](int vecVal,int findVal){ return vecVal < findVal+2 && vecVal >= findVal-2;});
    newVec1.removeRange(3,6);
    TEST_vec_print("newVec1 remove1 : ",newVec1);
    newVec1.removeFirst();
    newVec1.removeLast();
    newVec1.removeAt(3);
    TEST_vec_print("newVec1 remove2    : ",newVec1);
    TEST_vec_exec("newVec1.size()     : ",newVec1.size());
    TEST_vec_exec("newVec1.capacity() : ",newVec1.capacity());
    newVec1.removeAll();
    TEST_vec_print("newVec1 removeAll  : ",newVec1);
    TEST_vec_exec("newVec1.size()     : ",newVec1.size());
    TEST_vec_exec("newVec1.capacity() : ",newVec1.capacity());

    newVec1 = newVec3;
    TEST_vec_print("newVec1=newVec3          : ",newVec1);
    newVec1.replace(3,333);
    TEST_vec_print("newVec1.replace(3,333)   : ",newVec1);
    newVec1.sort();
    TEST_vec_print("newVec1.sort()    : ",newVec1);
    newVec1.sort([](int a,int b){return a>b;});
    TEST_vec_print("newVec1.sort(a>b) : ",newVec1);

    TEST_vec_print("newVec3               : ",newVec3);
    newVec1.swap(newVec3);
    TEST_vec_print("newVec1.swap(newVec3) : ",newVec1);
    TEST_vec_print("newVec3               : ",newVec3);
    newVec1.swap(3,7);
    TEST_vec_print("newVec1.swap(3,7) : ",newVec1);

    TEST_vec_exec("newVec1.takeAt(3)   : ",newVec1.takeAt(3));
    TEST_vec_exec("newVec1.takeFirst() : ",newVec1.takeFirst());
    TEST_vec_exec("newVec1.takeLast()  : ",newVec1.takeLast());
    TEST_vec_exec("newVec1.value(7)    : ",newVec1.value(7));


    TEST_vec_print("newVec5       : ",newVec5);
    TEST_vec_print("stdSet        : ",stdSet);
    TEST_vec_print("stdList       : ",stdList);
    stdSet = newVec5.toStdSet();
    stdList = newVec5.toStdList();
    TEST_vec_print("stdSet        : ",stdSet);
    TEST_vec_print("stdList       : ",stdList);

    TEST_FUNC_ENDED;
}

}
