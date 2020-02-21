# 项目说明 

一般常用的C++函数、功能，按class分类封装，可windows及linux跨平台使用。   
本项目的文件，尽量会以header-only的形式编写，即后缀名为.hpp。  
各个类和函数之间，也尽量避免互相调用（所以可能有重复代码多处使用），以便使用时直接包含头文件即可。  
CSDN专栏：[https://blog.csdn.net/aguoxin/category_9713658.html](https://blog.csdn.net/aguoxin/category_9713658.html)

# 编译及运行

## windows环境
支持vs2019，参见：cpphelper.sln。  
支持qt，参见：cpphelper.pro。

## linux环境
编译，执行脚本：sh run.sh help  
运行，执行脚本：sh run.sh start  
其他脚本命令，参考：sh run.sh help  

# 文件列表
| 目录及文件名                  | 概要说明 |  
| ----------------------------- | -------- |
| ├── README.md                 | 项目说明文档 |
| ├── cpphelper                 | C++常用类和函数封装目录：[博客专栏](https://blog.csdn.net/aguoxin/category_9713658.html)|
| │   ├── command.hpp           | [用C++11改进命令模式](https://blog.csdn.net/aguoxin/article/details/104279964) |
| │   ├── library.hpp           | [动态库显式调用封装](https://blog.csdn.net/aguoxin/article/details/104265711) |
| │   ├── list.hpp              | [在原std::list基础上改进接口](https://blog.csdn.net/aguoxin/article/details/104416538) |
| │   ├── threadpool.hpp        | [用C++11实现半同步半异步线程池](https://blog.csdn.net/aguoxin/article/details/104353352) |
| │   └── vector.hpp            | [在原std::vector基础上改进接口](https://blog.csdn.net/aguoxin/article/details/104377293) |
| ├── cpphelper.pro             | Qt编译项目文件 |
| ├── cpphelper.pro.user        | Qt编译配置文件 |
| ├── cpphelper.sln             | VS编译解决方案 |
| ├── mydll                     | 测试动态库目录 |
| │   ├── mydll.cpp             | 动态库实现文件 |
| │   ├── mydll.h               | 动态库头文件 |
| │   ├── mydll.pro             | 动态库Qt项目文件 |
| │   ├── mydll.sh              | 动态库Linux脚本 |
| │   ├── mydll.vcxproj         | 动态库VS项目文件 |
| │   ├── mydll.vcxproj.filters | 动态库VS项目文件过滤器 |
| │   └── mydll.vcxproj.user    | 动态库VS项目配置文件 |
| ├── run.sh                    | 项目Linux脚本 |
| └── test                      | 项目测试目录 |
|     ├── main.cpp              | 测试代码main文件 |
|     ├── test.cpp              | 测试代码实现文件 |
|     ├── test.h                | 测试代码头文件 |
|     ├── test.pro              | Qt测试项目配置文件 |
|     ├── test.vcxproj          | VS测试项目文件 |
|     ├── test.vcxproj.filters  | VS测试项目文件过滤器 |
|     └── test.vcxproj.user     | VS测试项目配置文件 |  
 
