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
vs2019���룺
�����½�->����Ŀ->dlltest
    ��Ŀ->����->����->�������ͣ���̬��(.dll)

linux���룺
�����ű��ļ���sh dlltest.sh���ǵ����-fPIC��-shared������

�������У�
�����ǵö�̬��·��Ϊ����·�������ߵ�ǰ·����
�������ض�̬����-ldl��LD_LIBRARY_PATH.
*/
