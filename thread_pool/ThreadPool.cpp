#include <iostream>
#include "ThreadPool.h"

ThreadPool::ThreadPool():
    m_mut(),
    m_cond(),
    m_isStarted(false)
{
    cout << "init thread pool" << endl;
}

ThreadPool::~ThreadPool()
{
    cout << "exit thread pool" << endl;
    if( m_isStarted )
    {
        stop();
    }
}

void ThreadPool::start()
{
    cout << "start" << endl;
    m_isStarted = true;
    m_threads.reserve(NumOfThreads);
    for( int i = 0; i < NumOfThreads; ++i)
    {
        m_threads.push_back( new thread( &ThreadPool::ThreadLoop, this));
    }
}

void ThreadPool::stop()
{
    cout << "stop" << endl;
    m_isStarted = false;
    {
        unique_lock<mutex> lock(m_mut);
        Tasks null_queue;
        m_tasks.swap(null_queue);
        m_cond.notify_all();
    }

    for( auto& item : m_threads)
    {
        cout << "wait join" << endl;
        item->join();
        delete item;
    }
    m_threads.clear();
}

void ThreadPool::addTask( const Task& task)
{
    cout << "add tast" << endl;
    unique_lock<mutex> lock(m_mut);
    TaskPair taskPair( level2, task);
    m_tasks.push(taskPair);
    m_cond.notify_one();
}

void ThreadPool::addTask( const TaskPair& task)
{
    cout << "add task pair" << endl;
    unique_lock<mutex> lock(m_mut);
    m_tasks.push(task);
    m_cond.notify_one();
}

ThreadPool::Task ThreadPool::take()
{
    unique_lock<mutex> lock(m_mut);
    while ( m_tasks.empty() && m_isStarted )
    {
        m_cond.wait(lock);
    }

    Task task;

    if( m_isStarted == false )
    {
        return task;
    }

    task = m_tasks.top().second;
    m_tasks.pop();

    return task;
}

void ThreadPool::ThreadLoop()
{
    while( m_isStarted)
    {
        cout << "thread loop before take" << endl;
        Task task = take();
        if( task)
        {
            task();
        }
    }
}
