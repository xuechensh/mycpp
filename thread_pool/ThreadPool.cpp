#include <iostream>
#include "ThreadPool.h"

ThreadPool::ThreadPool(int thread_num):
    m_thread_num(thread_num),
    m_mut(),
    m_cond(),
    m_isStarted(false)
{
    cout << "init thread pool" << endl;
}

ThreadPool::~ThreadPool()
{
    cout << "exit thread pool" << endl;

    stop();

    Tasks null_queue;
    m_tasks.swap(null_queue);
}

void ThreadPool::start()
{
    cout << "start" << endl;

    unique_lock<mutex> lock(m_mut);
    if( m_isStarted == true) return;
    
    m_isStarted = true;
    lock.unlock();

    m_threads.reserve(m_thread_num);
    for( int i = 0; i < m_thread_num; ++i)
    {
        m_threads.push_back( new thread( &ThreadPool::ThreadLoop, this));
    }
}

void ThreadPool::stop()
{
    cout << "stop" << endl;

    unique_lock<mutex> lock(m_mut);
    if( m_isStarted == false) return;

    m_isStarted = false;
    m_cond.notify_all();
    lock.unlock();

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
    m_tasks.push(task);
    if( m_isStarted == true)
    {
        m_cond.notify_one();
    }
}

void ThreadPool::ThreadLoop()
{
    while(1)
    {
        unique_lock<mutex> lock(m_mut);
    
        while( m_tasks.empty() && m_isStarted)
        {
            m_cond.wait(lock);
        }

        if( m_isStarted == false )
        {
            break;
        }

        Task task = m_tasks.front();
        m_tasks.pop();

        task.cb( task.arg);
    }
}
