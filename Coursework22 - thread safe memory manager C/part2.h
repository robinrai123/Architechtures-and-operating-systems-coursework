//
// Created by Pinea on 02/12/2019.
//

#ifndef COURSEWORK2_PART2_H
#define COURSEWORK2_PART2_H

typedef struct Node {
    size_t size;           //size of node in bytes
    int allocated;          //process or hole
    struct Node *next;      //next pointer
    struct Node *prev;      //previous pointer
    //void *memoryAddress;
} Node;

extern void *(*alloc)(size_t);

void *nextFit(size_t size);

void *bestFit(size_t size);

void split(struct Node *bigEnoughHole, size_t processBlock);

void *nextFit(size_t size);

void *bestFit(size_t size);

void *worstFit(size_t size);

void *firstFit(size_t size);

void initialise(void *memory, size_t size, char *algorithm);

void merge(struct Node *current, struct Node *next);

void deallocate(void *memory);

void *allocate(size_t size);


#endif //COURSEWORK2_PART2_H
