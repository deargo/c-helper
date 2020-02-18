# 项目说明 

一般常用的C++函数、功能，按class分类封装，可windows及linux跨平台使用。  
CSDN博客：[https://blog.csdn.net/aguoxin/category_9713658.html](https://blog.csdn.net/aguoxin/category_9713658.html)

# 编译及运行

## windows环境
支持vs2019，参见：cpphelper.sln。  
支持qt，参见：cpphelper.pro。

## linux环境
编译，执行脚本：sh run.sh help  
运行，执行脚本：sh run.sh start  
其他脚本命令，参考：sh run.sh help  

# 代码列表
|文件名           |                                                                    |简要概述|
|-----------------|--------------------------------------------------------------------|--------|
|library.hpp      |[博客地址](https://blog.csdn.net/aguoxin/article/details/104265711) |动态库显式调用封装|  
|command.hpp      |[博客地址](https://blog.csdn.net/aguoxin/article/details/104279964) |用C++11改进命令模式| 
|threadpool.hpp   |[博客地址](https://blog.csdn.net/aguoxin/article/details/104353352) |用C++11实现半同步半异步线程池|  
|vector.hpp       |[博客地址](https://blog.csdn.net/aguoxin/article/details/104377293) |在原std::vector基础上改进接口|  
