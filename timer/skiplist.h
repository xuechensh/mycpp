#ifndef _SKIPLIST_H_
#define _SkIPLIST_H_

#define SKIPLIST_MAX_LEVEL (32)
#define SKIPLIST_RANDOM (0.25)

typedef struct skiplistNode{
    unsigned long score;
    struct skiplistNode *backward;
    struct skiplistLevel{
        struct skiplistNode *forward;
        unsigned long span;
    } level[];
} skiplistNode;

typedef struct skipList{
    struct skiplistNode *head, *tail;
    unsigned long length;
    int level;
} skipList

skipList* slCrete(void);
void slFree(skipList*);



#endif

