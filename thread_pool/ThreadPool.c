#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define LIST_PUSH( item, head, tail)\
    do{\
        item->next = head;\
        if( head != NULL) head->pre = item;\
        head = item;\
        if( tail == NULL) tail = item;\
    }while(0)

#define LIST_DEL( item, head, tail)\
    do{\
        if(item->next != NULL) item->next->pre = item->pre;\
        if(item->pre != NULL) item->pre->next = item->next;\
        if( item == head) head = item->next;\
        if( item == tail) tail = item->pre;\
        item->pre = item->next = NULL;\
    }while(0)

typedef struct THREAD
{
    pthread_t thread_id;

    struct THREAD *next;
    struct THREAD *pre;
}Thread;

typedef struct TASK
{
    void (*cb)(void* arg);
    void *arg;

    struct TASK *next;
    struct TASK *pre;
}Task;

typedef struct MANAGEMENT
{
    int is_started;

    Thread *threads_head;
    Thread *threads_tail;
    Task *tasks_head;
    Task *tasks_tail;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
}ThreadPool;

void* threadLoop(void* arg)
{
    ThreadPool *tp = (ThreadPool*)arg;
    while(1)
    {
        pthread_mutex_lock( &tp->mutex);
        if(tp->tasks_tail == NULL && tp->is_started)
        {
            pthread_cond_wait( &tp->cond, &tp->mutex);
        }

        if( tp->is_started == 0)
        {
            pthread_mutex_unlock( &tp->mutex);
            break;
        }

        Task *task = tp->tasks_tail;
        LIST_DEL( task, tp->tasks_head, tp->tasks_tail);

        pthread_mutex_unlock( &tp->mutex);

        task->cb(task->arg);
    }
    return NULL;
}

    
int threadPoolDestroy(ThreadPool *tp)
{
    if( tp == NULL )
    {
        perror("thread pool free tp is null");
        return -1;
    }

    pthread_mutex_lock( &tp->mutex);
    tp->is_started = 0;
    
    Task *temp = NULL;
    for( temp = tp->tasks_tail; temp != NULL; temp = tp->tasks_tail)
    {
        LIST_DEL( temp, tp->tasks_head, tp->tasks_tail);
        free( temp);
    }

    pthread_cond_broadcast( &tp->cond);
    pthread_mutex_unlock( &tp->mutex);
    
    Thread *thread = NULL;
    for( thread = tp->threads_tail; thread != NULL; thread = tp->threads_tail)
    {
        LIST_DEL( thread, tp->threads_head, tp->threads_tail);
        pthread_join( thread->thread_id, NULL);
        free( thread);
    }

    pthread_mutex_destroy( &tp->mutex);
    pthread_cond_destroy( &tp->cond);

    return 0;
}

ThreadPool* threadPoolCreate(int tpNum)
{
    ThreadPool *tp = NULL;
    tp = (ThreadPool*)malloc(sizeof(ThreadPool));
    if( tp == NULL)
    {
        perror("malloc thread pool ptr error");
        return NULL;
    }
    memset( tp, 0, sizeof(ThreadPool));
    
    if( tpNum < 1) tpNum = 1;
    
    pthread_mutex_init( &tp->mutex, NULL);
    pthread_cond_init( &tp->cond, NULL);

    tp->is_started = 1;

    int i;
    for( i = 0; i < tpNum; ++i)
    {
        Thread *thread = NULL;
        thread = (Thread*) malloc(sizeof(Thread));
        if( thread == NULL)
        {
            perror("malloc thread error");
            return NULL;
        }
        memset( thread, 0, sizeof(Thread));

        pthread_create( &thread->thread_id, NULL, threadLoop, (void*)tp);
        
        LIST_PUSH( thread, tp->threads_head, tp->threads_tail);
    }

    tp->tasks_head = tp->tasks_tail = NULL;
    
    return tp;
}

int addTask(ThreadPool* tp, Task* task)
{
    if( tp == NULL)
    {
        perror("add task tp is null");
        return -1;
    }

    if( task == NULL)
    {
        perror("add task task is null");
        return -1;
    }

    pthread_mutex_lock( &tp->mutex);
    LIST_PUSH( task, tp->tasks_head, tp->tasks_tail);
    pthread_cond_signal( &tp->cond);
    pthread_mutex_unlock( &tp->mutex);
    
    return 0;
}


/* next is test example*/
void* test(void* arg)
{
    int a = (int)arg;
    printf("out is %d\n", a);
    return NULL;
}

void main(int argc, char* argv[])
{
    ThreadPool* tp = threadPoolCreate(4);
    
    int i;
    for(i = 0; i < 10; ++i)
    {
        Task* ts = malloc(sizeof(Task));
        memset( ts, 0, sizeof(ts));
        ts->cb = test;
        ts->arg = i;
        addTask( tp, ts);
    }
    sleep(5);
    threadPoolDestroy( tp);
}
