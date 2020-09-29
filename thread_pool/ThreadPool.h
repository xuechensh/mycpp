#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <functional>
#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <queue>
#include <utility>
#include <condition_variable>

using namespace std;

struct Task
{
    string name;
    function<void(void*)> cb;
    void* arg;
};

class ThreadPool
{
public:
    ThreadPool(int thread_num = 3);
    ~ThreadPool();

    void start();
    void stop();

    void addTask(const Task&);

private:
    ThreadPool(const ThreadPool&);
    const ThreadPool& operator=(const ThreadPool&);

    void ThreadLoop();
    Task take();

    typedef vector<thread*> Threads;
    typedef queue< Task, list<Task>> Tasks;

    Threads m_threads;
    Tasks m_tasks;

    int m_thread_num;
    mutex m_mut;
    condition_variable m_cond;
    bool m_isStarted;
};

#endif

