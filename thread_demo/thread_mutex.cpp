#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

int g_iNum = 0;
mutex mut;

int change(int num)
{
    lock_guard<mutex> lock(mut);
    g_iNum = num;
    cout << "change num to " << g_iNum << endl;
}

void test01(void)
{
    vector<thread> threads;
    for( int i = 0; i < 10; ++i)
    {
        threads.push_back(thread(change, i+1));
    }

    for( auto item = threads.begin(); item != threads.end(); ++item)
    {
        if(item->joinable()) item->join();
    }
}

class SafeInt
{
public:
    SafeInt():num(0){}

    void SetNum(int n)
    {
        lock_guard<mutex> lock(mut);
        cout << "before set num num = " << num << endl;
        num = n;
        cout << "safe int = " << num << endl;
    }
private:
    int num;
    mutex mut;
};

void test02(void)
{
    SafeInt a;
    vector<thread> threads;
    for( int i = 0; i < 10 ; ++i)
    {
       threads.push_back(thread( &SafeInt::SetNum, &a, i));
    }
    
    for( auto item = threads.begin(); item != threads.end(); ++item)
    {
        if(item->joinable()) item->join();
    }

}

int main(int argc, char* argv[])
{
    test01();
    test02();
    return 0;
}

