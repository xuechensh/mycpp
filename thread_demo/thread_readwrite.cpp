#include <iostream>
#include <thread>
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <cstdlib>

using namespace std;

class SafeInt
{
public:
    void Read()
    {   
        int s;
        srand(s);

        for( int i = 0; i < 10; ++i)
        {
            this_thread::sleep_for(chrono::second(rand() % 10));
            shared_lock<shared_mutex> lock(m_mut);
            cout << "num = " << m_num << endl;
        }
    }

    void Write(int num)
    {
        int s;
        srand(s);
        
        this_thread::sleep_for(chrono::second(rand() % 10));
        unique_lock<shared_mutex> lock(m_mut);
        m_num = num;
        cout << "set num = " << m_num << endl;
    }
private:
    shared_mutex m_mut;
    int m_num;
};

void test01(void)
{
    SafeInt a;
    vector<thread> threads;

    for( int i = 0; i < 3; ++i)
    {
        threads.push_back(thread( &SafeInt::Write, &a, i));
    }
    for( int i = 0; i < 10 ; ++i)
    {
       threads.push_back(thread( &SafeInt::Read, &a));
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

