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

    //ͬ������
    template <typename Task>
    class SyncQueue
    {
    public:
        using Tasks = std::list<Task>;

        SyncQueue(int maxSize): m_maxSize(maxSize), m_needStop(false) { }
        ~SyncQueue(){ resize(0);}

        //����¼�
        bool put(const Task& task)
        {
            return add(task);
        }

        //����¼�
        bool put(Task && task)
        {
            //�����ڲ��ӿ�,��������ת��
            return add(std::forward<Task>(task));
        }

        //�Ӷ�����ȡ�¼�,ȡ�����¼�
        bool take(Tasks &tasks)
        {
            Locker locker(m_mutex);
            //���������κ�һ����ȴ�,������m_needStopΪtrue����Ϊ����Ҫ��ֹ�����Բ�����
            m_notEmpty.wait(locker, [this]{return (m_needStop || notEmpty()); });
            if (m_needStop)
            {
                return false;
            }

            tasks = std::move(m_tasks);
            m_notFull.notify_one();
            return true;
        }

        //ȡһ���¼�
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

        //��ֹͬ������
        void stop()
        {
            {
                //�������������Դ�������
                Guard guard(m_mutex);
                //����ֹ��־��Ϊtrue
                m_needStop = true;
            }

            //�������н���һһ��ֹ
            m_notFull.notify_all();
            m_notEmpty.notify_all();
        }

        //����Ϊ��
        bool empty()
        {
            Guard guard(m_mutex);
            return m_tasks.empty();
        }

        //����Ϊ��
        bool full()
        {
            Guard guard(m_mutex);
            return m_tasks.size() == m_maxSize;
        }

        //��������
        int capacity()
        {
            Guard guard(m_mutex);
            return m_maxSize;
        }

        //���д�С
        int size()
        {
            Guard guard(m_mutex);
            return m_tasks.size();
        }

        //���ö��д�С
        void resize(int maxSize)
        {
            stop();
            Locker locker(m_mutex);
            m_tasks.clear();
            m_maxSize = maxSize;
        }

        //��ն���
        void clear()
        {
            stop();
            Locker locker(m_mutex);
            m_tasks.clear();
        }

    private:
        //���в�Ϊ��
        bool notFull() const
        {
            bool full = (m_tasks.size() >= m_maxSize);
            if (full)
            {
                cout << "the queue is full, need wait..." << endl;
            }

            return !full;
        }

        //���в�Ϊ��
        bool notEmpty() const
        {
            bool empty = m_tasks.empty();
            if (empty)
            {
                cout << "asynchronous thread["<<this_thread::get_id() << "]: the queue is empty, need wait." << endl;
            }

            return !empty;
        }

        //��������������,����Ϊ������ֹ��־Ϊfalse������¼�
        bool add(Task && task)
        {
            Locker locker(m_mutex);
            //���������κ�һ����ȴ�,������m_needStopΪtrue����Ϊ����Ҫ��ֹ�����Բ�����
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
        //������
        Tasks m_tasks;
        //�����������������������ʹ��
        std::mutex m_mutex;
        //���в�Ϊ�յ���������
        std::condition_variable m_notEmpty;
        //���в�Ϊ������������
        std::condition_variable m_notFull;
        //ͬ��������󳤶�
        size_t m_maxSize;
        //��ֹ�ı�ʶ,��Ϊtrueʱ����ͬ������Ҫ��ֹ
        bool m_needStop;
    };

private:
    using Ret = void;
    using Task = std::function<Ret()>;
    using Tasks = std::list<Task>;

public:
    //���ݸ�ͬ�����е�������
    enum { DEFAULT_MAX_TASK_COUNT = 1000 };

    //���캯��,Ĭ�ϴ����߳���ΪCPU��������
    CThreadPool(int handleTheadCount = std::thread::hardware_concurrency()):
        m_handle_count(handleTheadCount),m_queue(CThreadPool::DEFAULT_MAX_TASK_COUNT)
    {
        start();
    }

    ~CThreadPool()
    {
        //���û��ֹͣ��������ֹͣ�̳߳�
        stop();
    }

    //�����̳߳أ�Ĭ����������
    void start()
    {
        Locker locker(m_mutex);
        if(m_running)
        {
            return;
        }
        run();
    }

    //��ֹ���������ִ��
    void stop()
    {
        Locker locker(m_mutex);
        if(false == m_running)
        {
            return;
        }

        //��ֹͬ������
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

    //������񣺽��ܷ�������Ϊvoid���͵�function����������lamda����ͨ����
    template< class Func, class... Args, class = typename std::enable_if<!std::is_member_function_pointer<Func>::value>::type>
    bool addTask(Func && func, Args && ... args)
    {
        return m_queue.put([&func, args...]{return func(args...); });
    }

    //������񣺽��ܷ�������Ϊvoid���͵�const��Ա����
    template<class CObj, class... DArgs, class PObj, class... Args>
    bool addTask(Ret(CObj::*func)(DArgs...) const, PObj && pObj, Args && ... args)
    {
        return m_queue.put([&, func]{return (*pObj.*func)( args...); });
    }

    //������񣺽��ܷ�������Ϊvoid���͵�non-const���ͳ�Ա����
    template<class CObj, class... DArgs, class PObj, class... Args>
    bool addTask(Ret(CObj::*func)(DArgs...), PObj && pObj, Args && ... args)
    {
        return m_queue.put([&, func]{return (*pObj.*func)( args...); });
    }

    //���нӿ�
    bool queueEmpty()  { return m_queue.empty();}
    bool queueFull()  { return m_queue.full();}
    int queueCapacity()  { return m_queue.capacity();}
    int queueSize()  { return m_queue.size();}
    void queueResize(int maxSize)  { return m_queue.resize(maxSize);}
    void queueClear()  { return m_queue.clear();}

    //����ӿ�
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
        //����numThreads�������߳���
        for (int i = 0; i < m_handle_count; i++)
        {
            //����߳����εĴ���
            m_threadgroup.emplace_back(std::make_shared<std::thread>(&CThreadPool::runInThread, this));
        }
    }
    void runInThread()
    {
        while (m_running)
        {
            Tasks list;
            m_queue.take(list);            
            //ȡ����������е�ȫ��,����ִ��
            for (auto & task : list)
            {
                if (!m_running)
                {
                    return ;
                }
                //ִ������
                task();
            }
        }
        return;
    }

private:
    std::mutex m_mutex;
    int m_handle_count;

    //����������߳���
    std::list<std::shared_ptr<std::thread>> m_threadgroup;
    //ͬ������
    SyncQueue<Task> m_queue;
    //���еı�־,flase������ֹ
    atomic_bool m_running;
    //��֤�ں����ڶ��̻߳�����ֻ������һ��
    std::once_flag m_flag;
};

}
