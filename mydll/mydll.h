#pragma once 

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif


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

/*
vs2019编译：
　　新建->空项目->dlltest
    项目->属性->常规->配置类型：动态库(.dll)

linux编译：
　　脚本文件：sh dlltest.sh。记得添加-fPIC和-shared参数。

测试运行：
　　记得动态库路径为绝对路径，或者当前路径。
　　加载动态库需-ldl和LD_LIBRARY_PATH.
*/
