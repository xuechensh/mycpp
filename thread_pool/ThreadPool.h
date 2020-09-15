#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <functional>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include <utility>
#include <condition_variable>

using namespace std;

class ThreadPool
{
public:
    static const int NumOfThreads = 3;
    enum TaskPriE { level0, level1, level2};
    typedef function<void()> Task;
    typedef pair<TaskPriE, Task> TaskPair;
    
    ThreadPool();
    ~ThreadPool();

    void start();
    void stop();

    void addTask(const Task&);
    void addTask(const TaskPair&);

private:
    ThreadPool(const ThreadPool&);
    const ThreadPool& operator=(const ThreadPool&);

    struct TaskPriComp
    {
        bool operator()(const TaskPair& p1, const TaskPair& p2)
        {
            return p1.first > p2.first;
        }
    };

    void ThreadLoop();
    Task take();

    typedef vector<thread*> Threads;
    typedef priority_queue< TaskPair, vector<TaskPair>, TaskPriComp> Tasks;

    Threads m_threads;
    Tasks m_tasks;

    mutex m_mut;
    condition_variable m_cond;
    bool m_isStarted;
};

#endif

