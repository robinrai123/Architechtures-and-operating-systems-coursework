//Robin Rai and Hemal Munbodh
//Last updated on 09/12/2019
//Version 6.9
#ifndef COURSEWORK2_PART1_H
#define COURSEWORK2_PART1_H


typedef struct Node {
    size_t size;           //size of node in bytes
    int allocated;          //process or hole
    struct Node *next;      //next pointer
    struct Node *prev;      //previous pointer
    //void *memoryAddress;
} Node;

void initialise(void *memory, size_t size);

void *allocate(size_t size);

void split(struct Node *bigEnoughHole, size_t processBlock);

void deallocate(void *memory);

void merge(struct Node *current, struct Node *next);


void nodePrinter();

void repeatTest();

void initialiseTest();

void allocateTest();

void deallocateTest();


#endif //COURSEWORK2_PART1_H
