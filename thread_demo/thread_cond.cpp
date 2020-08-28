#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>

using namespace std;

class SafeDeque
{
public:
    void Add()
    {
        for( int i = 0; i < 10; ++i)
        {
            unique_lock<mutex> lock(m_mut);
            m_q.push_front(i);
            lock.unlock();
            cout << "add num= " << i << endl;
            m_cond.notify_one();
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    void Del()
    {
        unique_lock<mutex> lock(m_mut);
        while( m_q.empty() )
        {
            m_cond.wait( lock);
        }

        int num = m_q.back();
        m_q.pop_back();
        
        cout << "del num= " << num << endl;
    }
private:
    deque<int> m_q;
    mutex m_mut;
    condition_variable m_cond;
};

void test01(void)
{
    SafeDeque a;
    vector<thread> threads;

    threads.push_back(thread( &SafeDeque::Add, &a));

    for( int i = 0; i < 10 ; ++i)
    {
       threads.push_back(thread( &SafeDeque::Del, &a));
    }
    
    for( auto item = threads.begin(); item != threads.end(); ++item)
    {
        if(item->joinable()) item->join();
    }

}

int main(int argc, char* argv[])
{
    test01();
    return 0;
}

