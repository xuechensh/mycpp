#include <iostream>
#include <mutex>
#include <unistd.h>
#include "ThreadPool.h"

using namespace std;

mutex mut;
static int a = 0;

void func()
{
    unique_lock<mutex> lock(mut);
    ++a;
    cout << "change a to " << a << endl;
}
    

int main(int argc, char* argv[])
{
    ThreadPool tp;
    tp.start();

    for( int i = 0; i < 10; ++i)
    {
        ThreadPool::Task a = func;
        tp.addTask(a);
    }

    sleep(10);

}
