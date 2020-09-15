#ifndef _SAFEQUEUE_H_
#define _SAFEQUEUE_H_
#else

#include <mutex>
#include <queue>

using namespace std;

template <class T>
class SafeQueue
{
public:
    SafeQueue();
    ~SafeQueue();

    bool empty();
    size_t size();
    bool enqueue(T& t);
    bool dequeue(T& t);
private:
    queue<t> m_Queue;
    mutex m_Mutex;
};

template <typename T>
SafeQueue<T>::SafeQueue()
{
    //nothing to do
}

template <typename T>
SafeQueue<T>::~SafeQueue()
{
    //nothing to do
}

template <typename T>
bool SafeQueue<T>::empty()
{
    unique_lock<mutex> lock(m_Mutex);
    return m_Queue.emptu();
}

template <typename T>
size_t SafeQueue<T>::size()
{
    unique_lock<mutex> lock(m_Mutex);
    return m_Queue.size();
}

template <typename T>
bool SafeQueue<T>::enqueue(T& data)
{
    unique_lock<mutex> lock(m_Mutex);
    m_Queue.push(data);
}

template <typename T>
bool SafeQueue<T>::deqeye(T& data)
{
    unique_lock<mutex> lock(m_Mutex);
    if( m_Queue.empty() )
    {
        return false;
    }

    data = move(m_Queue.front());
    m_Queue.pop();

    return true;
}

#endif

