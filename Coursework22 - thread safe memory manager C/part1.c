//Robin Rai (100242165) and Hemal Munbodh (100246776)
//Last updated on 09/12/2019
//Version 6.9
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "part1.h"

//Node defined in h file to prevent compiler from whining
Node *firstN;  //constant node that's used as the first node

void initialise(void *memory, size_t size) {
    if (size <= sizeof(Node)) {    //requires heap to be big enough for a node
        printf("Too little memory to initialise.\n");
        return;
    }
    //initialises the constant node to be the size of the heap - the size of the node
    firstN = (Node *) memory;
    firstN->size = size - sizeof(struct Node);
    firstN->allocated = 0;  //Set to a hole
    firstN->next = NULL;    //it's the only node present, so NULL next and previous
    firstN->prev = NULL;
}

void *allocate(size_t size) {
    printf("Attempting to allocate %i bytes.\n", (int) size);
    if ((int) size <= 0) {  //casts to signed int so it can check if negative
        printf("Error: Can't allocate 0/negative bytes.\n");
        return NULL;
    }
    struct Node *current, *previous, *result;
    current = firstN;   //current is now the first Node in the heap
    while (current != NULL) {   //goes through all nodes in the heap
        if (current->allocated == 1) {  //if a process, skip
            current = current->next;
            continue;
        }
        if ((current->size) == size) {  //if exact fit
            current->allocated = 1;    //set flag to allocated
            result = (current + sizeof(Node));  //sets reutrn result to actual memory block
            printf("Allocated with exact fit.\n");
            return result;
        } else if ((current->size) > (size + sizeof(Node))) {  //if size + the size of a new node fits
            split(current, size);    //split the hole into two, allocating the first hole
            result = (current + sizeof(Node));  //sets return result to actual memory block
            printf("Allocated with split.\n");
            return result;
        } else if (current->size >= size) {  //if size fits in the current hole
            current->allocated = 1;        //set flag to allocated
            printf("Allocated with extra memory.\n");
            result = current + sizeof(Node);  //sets return result to actual memory block
            return result;    //returns the entire hole's worth of memory instead of splitting.
        } else {    //if didn't fit, go to the next node
            current = current->next;
        }
    }
    //the while loop has ended so there was no hole big enough.
    result = NULL;
    if (result == NULL) {
        printf("No free space to allocate.\n");
    }
    return result;
}

void split(struct Node *bigEnoughHole, size_t processBlock) {  //takes in a node and splits it into two
    //firstNode's size part being the size given, and the other the remainder
    struct Node *newHoleNode = (void *) ((void *) bigEnoughHole + processBlock +
                                         sizeof(struct Node));  //makes second node (hole)
    newHoleNode->size =
            (bigEnoughHole->size) - processBlock - sizeof(struct Node);  //sets the size to the remaining space
    newHoleNode->allocated = 0;  //sets to a hole
    newHoleNode->next = bigEnoughHole->next;  //secondNode's next is now firstNode's
    newHoleNode->prev = bigEnoughHole;  //secondNode's prev is now firstNode
    bigEnoughHole->size = processBlock;  //firstNode's size is set to what it was split to
    bigEnoughHole->allocated = 1;  //sets firstNode to a process
    bigEnoughHole->next = newHoleNode;  //firstNode's next is now secondNode
    if (newHoleNode->next == NULL) {    //if newHoleNode's next is NULL exit function
        return;
    } else {
        newHoleNode->next->prev = newHoleNode;  //set the newHole next's prev to newHole
    }
    //bigEnoughHole's prev does not change.
}

void deallocate(void *memory) {
//gets memory's node and deallocates it. Runs through heap and merges any holes

    Node *memoryNode = memory;
    Node *current = memoryNode - sizeof(Node);
    if (current->allocated != 1) {  //input node must be a process
        printf("Error, invalid deallocate location or already deallocated.\n");
        return;
    }
    current->allocated = 0;  //sets to hole
    current = firstN;  //starts from first node
    Node *next;
    while (current != NULL) {  //goes through all nodes
        next = current->next;  //sets next node to be current's next node
        if (next == NULL) {  //if at end of heap and next is null, skip to end
            current = current->next;
            continue;
        }
        if (current->allocated == 0 && next->allocated == 0) {//if current and next are holes
            merge(current, next);  //merge them together
        } else {
            current = current->next;  //if they're not, skip to next nodes
        }
    }
    printf("Deallocated successfully.\n");
}

void merge(Node *current, Node *next) {  //takes two nodes and merges them

    current->size += (next->size) + sizeof(Node);  //result is the size of both sizes plus the size of a node
    current->next = next->next;  //result's next node is what next node's next is
    if (current->next == NULL) {    //if current->next is NULL end function
        return;
    } else {
        current->next->prev = current;  //set the node after current's prev to current.
    }
}






//test harnesses
/*

***WARNING***

TEST RESULTS ASSUME 4 BYTE INTEGERS

***WARNING***
*/
void nodePrinter() {
//prints out all nodes in heap
    Node *current = firstN;  //starts from beginning
    int counter = 1;
    printf("\n");
    while (current != NULL) {  //goes through all nodes
        printf("||Node: %i, Allocated: %i, Size: %i||\n", counter, current->allocated, current->size);
        current = current->next;
        counter++;  //increments current node
    }
    printf("\n");
}

void repeatTest() { //allocates a bunch then deallocates at random. MAKE SURE THERES ENOUGH MEMORY IN INITIALISE
    printf("\n\nREPEAT TESTING \n");
    int *ptr = (int *) malloc(3000);
    initialise(ptr, 3000);
    int arraySize = 50;
    void *nodeArray[arraySize];
    printf("State of initial heap:\n");
    nodePrinter();
    for (int i = 0; i < arraySize; i++)
        //make an array of allocate pointers, then deallocate them at random
    {
        nodeArray[i] = allocate(10);
    }
    nodePrinter();
    int array[arraySize];
    for (int i = 0; i < arraySize; i++) {
        array[i] = i;
    }
    for (int i = 0; i < arraySize; i++) {
        int temp = array[i];
        int randomIndex = rand() % arraySize;
        array[i] = array[randomIndex];
        array[randomIndex] = temp;
    }
    for (int i = 0; i < arraySize; i++) {    // print array
        printf("Attempting to deallocate: %d\n", array[i]);
        deallocate(nodeArray[array[i]]);
    }
    nodePrinter();      //should be one node;
}

void initialiseTest() {
    printf("\n\nINITIALISE TESTING \n");
    int *ptr = (int *) malloc(200);
    initialise(ptr, 200000);    //very bad situation, unsure how to check and stop
    nodePrinter();

    int *ptr2 = (int *) malloc(2000);
    initialise(ptr, 200);    //less bad situation, memory goes unused
    nodePrinter();

    int *ptr3 = (int *) malloc(2000);
    initialise(ptr, 2000);    //normal usage
    nodePrinter();
}

void allocateTest() {
    printf("\n\nALLOCATE TESTING \n");
    int *ptr = (int *) malloc(420);
    initialise(ptr, 420);
    printf("State of initial heap:\n");
    nodePrinter();
    void *hole1 = allocate(50);
    allocate(10);
    void *hole2 = allocate(70);
    allocate(10);
    void *hole3 = allocate(120);
    deallocate(hole1);
    deallocate(hole2);
    deallocate(hole3);
    printf("State of heap at start of testing:\n");
    nodePrinter();

    printf("Allocate testing: \n");
    allocate(500);  //should fail
    allocate(500);  //should fail
    allocate(0);    //should fail
    allocate(-1);   //should fail
    allocate(50);   //exact fit test
    allocate(69);   //should allocate with extra memory
    allocate(69);   //should allocate with split

    nodePrinter();
}

void deallocateTest() {
    printf("\n\nDEALLOCATE TESTING \n");
    int *ptr = (int *) malloc(420);
    initialise(ptr, 420);
    printf("State of initial heap:\n");
    nodePrinter();
    void *hole1 = allocate(50);
    void *hole2 = allocate(50);
    void *hole3 = allocate(50);
    printf("State of heap at start of testing:\n");
    nodePrinter();

    deallocate(hole2);      //testing process hole process senario
    nodePrinter();
    hole2 = allocate(50);


    deallocate(hole1);
    deallocate(hole2);      //testing hole hole process senario
    nodePrinter();

    hole1 = allocate(50);
    hole2 = allocate(50);

    deallocate(hole1);
    deallocate(hole2);
    deallocate(hole3);      //testing hole hole hole senario
    nodePrinter();

    deallocate(hole1);                  //already deallocated testing - Should fail and complain
    deallocate(hole1 + 90);   //bad location testing - Should fail and complain
    nodePrinter();
}


int main() {
    initialiseTest();
    repeatTest();
    allocateTest();
    deallocateTest();
    printf("\n\nTESTING FINISHED. \n");
}