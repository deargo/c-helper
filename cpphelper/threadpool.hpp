#pragma once

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <atomic>
#include <functional>

using namespace std;

namespace CppHelper
{
class CThreadPool
{
private:
    using Locker = std::unique_lock<std::mutex>;
    using Guard = std::lock_guard<std::mutex>;

    //同步队列
    template <typename Task>
    class SyncQueue
    {
    public:
        using Tasks = std::list<Task>;

        SyncQueue(int maxSize): m_maxSize(maxSize), m_needStop(false) { }
        ~SyncQueue(){ resize(0);}

        //添加事件
        bool put(const Task& task)
        {
            return add(task);
        }

        //添加事件
        bool put(Task && task)
        {
            //调用内部接口,进行完美转发
            return add(std::forward<Task>(task));
        }

        //从队列中取事件,取所有事件
        bool take(Tasks &tasks)
        {
            Locker locker(m_mutex);
            //当不满足任何一个则等待,但是若m_needStop为true是因为任务要终止了所以不阻塞
            m_notEmpty.wait(locker, [this]{return (m_needStop || notEmpty()); });
            if (m_needStop)
            {
                return false;
            }

            tasks = std::move(m_tasks);
            m_notFull.notify_one();
            return true;
        }

        //取一个事件
        bool take(Task &task)
        {
            Locker locker(m_mutex);
            m_notEmpty.wait(locker, [this]{return m_needStop || notEmpty(); });
            if (m_needStop)
            {
                return false;
            }

            task = m_tasks.front();
            m_tasks.pop_front();
            m_notFull.notify_one();
            return true;
        }

        //终止同步队列
        void stop()
        {
            {
                //锁作用域就在这对大括号内
                Guard guard(m_mutex);
                //将终止标志设为true
                m_needStop = true;
            }

            //唤醒所有进程一一终止
            m_notFull.notify_all();
            m_notEmpty.notify_all();
        }

        //队列为空
        bool empty()
        {
            Guard guard(m_mutex);
            return m_tasks.empty();
        }

        //队列为满
        bool full()
        {
            Guard guard(m_mutex);
            return m_tasks.size() == m_maxSize;
        }

        //队列容量
        int capacity()
        {
            Guard guard(m_mutex);
            return m_maxSize;
        }

        //队列大小
        int size()
        {
            Guard guard(m_mutex);
            return m_tasks.size();
        }

        //重置队列大小
        void resize(int maxSize)
        {
            stop();
            Locker locker(m_mutex);
            m_tasks.clear();
            m_maxSize = maxSize;
        }

        //清空队列
        void clear()
        {
            stop();
            Locker locker(m_mutex);
            m_tasks.clear();
        }

    private:
        //队列不为满
        bool notFull() const
        {
            bool full = (m_tasks.size() >= m_maxSize);
            if (full)
            {
                cout << "the queue is full, need wait..." << endl;
            }

            return !full;
        }

        //队列不为空
        bool notEmpty() const
        {
            bool empty = m_tasks.empty();
            if (empty)
            {
                cout << "asynchronous thread["<<this_thread::get_id() << "]: the queue is empty, need wait." << endl;
            }

            return !empty;
        }

        //向队列中添加任务,若不为满且终止标志为false则添加事件
        bool add(Task && task)
        {
            Locker locker(m_mutex);
            //当不满足任何一个则等待,但是若m_needStop为true是因为任务要终止了所以不阻塞
            m_notFull.wait(locker, [this]{return m_needStop || notFull(); });
            if (m_needStop)
            {
                return false;
            }

            m_tasks.emplace_back(std::forward<Task>(task));
            m_notEmpty.notify_one();
            return true;
        }

    private:
        //缓冲区
        Tasks m_tasks;
        //互斥量和条件变量结合起来使用
        std::mutex m_mutex;
        //队列不为空的条件变量
        std::condition_variable m_notEmpty;
        //队列不为满的条件变量
        std::condition_variable m_notFull;
        //同步队列最大长度
        size_t m_maxSize;
        //终止的标识,当为true时代表同步队列要终止
        bool m_needStop;
    };

private:
    using Ret = void;
    using Task = std::function<Ret()>;
    using Tasks = std::list<Task>;

public:
    //传递给同步队列的最大个数
    enum { DEFAULT_MAX_TASK_COUNT = 1000 };

    //构造函数,默认处理线程数为CPU核心数量
    CThreadPool(int handleTheadCount = std::thread::hardware_concurrency()):
        m_handle_count(handleTheadCount),m_queue(CThreadPool::DEFAULT_MAX_TASK_COUNT)
    {
        start();
    }

    ~CThreadPool()
    {
        //如果没有停止，则主动停止线程池
        stop();
    }

    //启动线程池（默认已启动）
    void start()
    {
        Locker locker(m_mutex);
        if(m_running)
        {
            return;
        }
        run();
    }

    //终止所有任务的执行
    void stop()
    {
        Locker locker(m_mutex);
        if(false == m_running)
        {
            return;
        }

        //终止同步队列
        m_queue.stop();
        m_running = false;

        for (auto thread : m_threadgroup)
        {
            if (thread)
            {
                thread->join();
            }
        }
        m_threadgroup.clear();
    }

    //添加任务：接受返回类型为void类型的function、函数对象、lamda和普通函数
    template< class Func, class... Args, class = typename std::enable_if<!std::is_member_function_pointer<Func>::value>::type>
    bool addTask(Func && func, Args && ... args)
    {
        return m_queue.put([&func, args...]{return func(args...); });
    }

    //添加任务：接受返回类型为void类型的const成员函数
    template<class CObj, class... DArgs, class PObj, class... Args>
    bool addTask(Ret(CObj::*func)(DArgs...) const, PObj && pObj, Args && ... args)
    {
        return m_queue.put([&, func]{return (*pObj.*func)( args...); });
    }

    //添加任务：接受返回类型为void类型的non-const类型成员函数
    template<class CObj, class... DArgs, class PObj, class... Args>
    bool addTask(Ret(CObj::*func)(DArgs...), PObj && pObj, Args && ... args)
    {
        return m_queue.put([&, func]{return (*pObj.*func)( args...); });
    }

    //队列接口
    bool queueEmpty()  { return m_queue.empty();}
    bool queueFull()  { return m_queue.full();}
    int queueCapacity()  { return m_queue.capacity();}
    int queueSize()  { return m_queue.size();}
    void queueResize(int maxSize)  { return m_queue.resize(maxSize);}
    void queueClear()  { return m_queue.clear();}

    //处理接口
    bool isRunning() { Guard guard(m_mutex); return m_running;}
    int handleCount() {Guard guard(m_mutex); return  m_handle_count;}
    void handleCountResize(int handleTheadCount)
    {
        stop();
        Locker locker(m_mutex);
        m_handle_count = handleTheadCount;
        run();
    }


private:
    void run()
    {
        m_running  = true;
        //建立numThreads个数的线程组
        for (int i = 0; i < m_handle_count; i++)
        {
            //多个线程依次的处理
            m_threadgroup.emplace_back(std::make_shared<std::thread>(&CThreadPool::runInThread, this));
        }
    }
    void runInThread()
    {
        while (m_running)
        {
            Tasks list;
            m_queue.take(list);            
            //取出任务队列中的全部,依次执行
            for (auto & task : list)
            {
                if (!m_running)
                {
                    return ;
                }
                //执行任务
                task();
            }
        }
        return;
    }

private:
    std::mutex m_mutex;
    int m_handle_count;

    //处理任务的线程组
    std::list<std::shared_ptr<std::thread>> m_threadgroup;
    //同步队列
    SyncQueue<Task> m_queue;
    //运行的标志,flase代表终止
    atomic_bool m_running;
    //保证在函数在多线程环境中只被调用一次
    std::once_flag m_flag;
};

}
