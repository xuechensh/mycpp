#include "skiplist.h"
#include <stdlib.h>

static int slRandomLevel(void){
    int level = 1;

    if( (random() & 0xffff) < ( SKIPLIST_RANDOM * 0xffff) ){
        level += 1;
    }

    return ( level < SKIPLIST_MAX_LEVEL ) ? level : SKIPLIST_MAX_LEVEL;
}

static skiplistNode* slCreateNode(int level, unsigned long score){
    skiplistNode *node;
    node = (skiplistNode*) malloc( sizeof(*node) + level * sizeof(struct skiplistLevel));
    if( node == NULL){
        return node;
    }
    memset( node, 0, sizeof(*node));

    node->score = score;

    return node;
}

static void slFreeNode( skiplistNode* node){
    if( node == NULL){
        return;
    }

    free( node);

    return;
}

skipList* slCreateList(void){
    skipList *list = NULL;

    skiplistNode *head = slCreateNode(SKIPLIST_MAX_LEVEL, 0);
    if( head == NULL ){
        return list;
    }

    list = malloc( sizeof(*list));
    if( list == NULL){
        return list;
    }
    memset( list, 0, sizeof(*list));

    list->level = 1;
    list->length = 0;
    list->head = head;
    list->tail = NULL;

    return list;
}


void slFreeList(skipList* list){
    if( list == NULL){
        return;
    }

    skiplistNode *curr = list->head->level[0].forward, next;

    slFreeNode( list->head);
    while( curr ){
        next = curr->level[0].forward;
        slFreeNode( curr);
        curr = next;
    }

    free( list);

    return;
}


