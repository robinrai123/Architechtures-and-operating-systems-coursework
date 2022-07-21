//Robin Rai (100242165) and Hemal Munbodh (100246776)
//Last updated on 09/12/2019
//Version 6.9
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>
#include "part2.h"

//Node defined in h file to prevent compiler from whining
Node *firstN;
Node *trackN;

void initialise(void *memory, size_t size, char *algorithm) {
    if (size <= sizeof(Node)) {    //requires heap to be big enough for a node
        printf("Too little memory to initialise.\n");
        return;
    }
    //initialises the constant node to be the size of the heap - the size of the node
    firstN = (Node *) memory;
    firstN->size = size - sizeof(Node);
    firstN->allocated = 0;  //sets to a hole
    firstN->next = NULL;    //next and previous are set to NULL since there's one node
    firstN->prev = NULL;
    trackN = firstN;        //sets nextFits tracking node to the start
    char first[] = "FirstFit";
    char next[] = "NextFit";
    char best[] = "BestFit";
    char worst[] = "WorstFit";

    if (strcmp(algorithm, first) == 0) {    //sets funtion pointer to specified function
        alloc = &firstFit;
        printf("Using first fit.\n");
    } else if (strcmp(algorithm, next) == 0) {
        alloc = &nextFit;
        printf("Using next fit.\n");
    } else if (strcmp(algorithm, best) == 0) {
        alloc = &bestFit;
        printf("Using best fit.\n");
    } else if (strcmp(algorithm, worst) == 0) {
        alloc = &worstFit;
        printf("Using worst fit.\n");
    } else {    //if bad input defaults to firstfit
        alloc = &firstFit;
        printf("Defaulting to first fit.\n");
    }


}

void *(*alloc)(size_t);

void *allocate(size_t size) {    //allocate calls function pointer set by initialise
    (*alloc)(size);
}

void split(struct Node *bigEnoughHole, size_t processBlock) {  //takes in a node and splits it into two
  //firstNode's size part being the size given, and the other the remainder
    struct Node *newHoleNode = (void *) ((void *) bigEnoughHole + processBlock + sizeof(struct Node));  //makes second node (hole)
    newHoleNode->size = (bigEnoughHole->size) - processBlock - sizeof(struct Node);  //sets the size to the remaining space
    newHoleNode->allocated = 0;  //sets to a hole
    newHoleNode->next = bigEnoughHole->next;  //secondNode's next is now firstNode's
    newHoleNode->prev = bigEnoughHole;  //secondNode's prev is now firstNode
    bigEnoughHole->size = processBlock;  //firstNode's size is set to what it was split to
    bigEnoughHole->allocated = 1;  //sets firstNode to a process
    bigEnoughHole->next = newHoleNode;  //firstNode's next is now secondNode
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
            printf("Merged.\n");
        } else {
            current = current->next;  //if they're not, skip to next nodes
        }
    }
    printf("Deallocated successfully.\n");
}

void merge(Node *current, Node *next) {//takes two nodes and merges them
    if (next == trackN) {
        trackN = current;    //sets tracking node to output/surviving node
    }
    current->size += (next->size) + sizeof(Node);  //result is the size of both sizes plus the size of a node
    current->next = next->next;  //result's next node is what next node's next is
}

void *firstFit(size_t size) {
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

void *nextFit(size_t size) {  //uses first fit but starts from trackN instead of firstN
    printf("Attempting to allocate %i bytes.\n", (int) size);
    if ((int) size <= 0) {  //casts to signed int so it can check if negative
        printf("Error: Can't allocate 0/negative bytes.\n");
        return NULL;
    }
    struct Node *current, *result;
    int looped = 0;    //flag if trackN had looped around to start of heap
    if (trackN == NULL) {  //if trackN has been advanced to end of heap
        trackN = firstN;  //set to start of heap
    }
    current = trackN;    //current is trackN
    while ((looped == 0) || (looped == 1 && current != trackN)) {   //goes through all nodes,
    //stops once it has looped and reached the node it started from.
        if (current == NULL) {  //if reached end of heap
            current = firstN;  //rest current to firstNode
            looped = 1;  //set looped flag to 1
            continue;  //skip everything else instead of using else if for efficiency
        }
        if (current->allocated == 1) {  //skip if node is allocated
            current = current->next;

            continue;
        }
        if ((current->size) == size) {  //if exact fit
            current->allocated = 1;    //set node to process
            result = (current + sizeof(Node));  //result is actual block of memory
            printf("Allocated with exact fit.\n");
            trackN = current->next;    //advances tracked Node
            return result;
        } else if ((current->size) > (size + sizeof(Node))) {  //if size + size of new node fits in current hole
            split(current, size);  //split into two
            result = (current + sizeof(Node));  //result is actual block of memory
            printf("Allocated with split.\n");
            trackN = current->next;  //advances tracked node
            return result;
        } else if (current->size >= size) {  //if size fits in current hole, allocates with extra memory
            current->allocated = 1;    //set to process
            printf("Allocated with extra memory.\n");
            result = current + sizeof(Node);  //result is actual block of memory
            trackN = current->next;  //advances tracked node
            return result;  //returns whole hole
        } else {  //no space in current, skip
            current = current->next;

        }
    }
    //end of while loop, so no suitable hole found
    printf("Error: No free space.\n");
    result = NULL;
    return result;
}

void *bestFit(size_t size) {  //allocates to smallest gap possible
    printf("Attempting to allocate %i bytes.\n", (int) size);
    if ((int) size <= 0) {  //fails if input is 0 or less
        printf("Error: Can't allocate 0/negative bytes.\n");
        return NULL;
    }
    Node *current = firstN;
    Node *result;
    int minimum = INT_MAX;  //minimum is the biggest number
    int counter = 0;        //flag if suitable hole found
    int includeNode = 0;    //flag if suitable hole for size + sizeof Node found
    while (current != NULL) {  //goes through all nodes
        if (current->allocated == 1) {  //if process, skip node
            current = current->next;
            continue;
        }
        if (current->size == size) {  //if exact fit, allocate and return straight away
            printf("Allocated with exact fit.\n");
            current->allocated = 1;  //sets to process
            return current + sizeof(Node);  //result is acutal block of memory
        }
        if (((current->size) - (size + sizeof(Node))) <= minimum &&
            (((int) current->size - (int) (size + sizeof(Node)) > 0))) {
            //if the new gap between hole and allocation is smallest
            result = current;  //result is current node
            minimum = (current->size) - (size + sizeof(Node)); //sets minimum to new minimum gap

            counter++;  //increments counter - hole has been found
            includeNode = 1;  //increments nodeCounter - hole for size and node has been found
        }
        if ((current->size - size) <= minimum && (((int) current->size - (int) size) > 0)) {
        //if the new gap is the smallest
            result = current;  //result is current node
            minimum = (current->size) - size;  //sets new minimum

            counter++;  //increments counter - hole has been found
            includeNode = 0;  //resets nodeCounter - use this result since gap is smaller

        }
        current = current->next;  //move on to next node

    }
    if (counter == 0) {  //no suitable hole was found
        printf("Error: No space to allocate.\n");
        return NULL;
    } else if (includeNode == 1) {  //result is size + node
        printf("Allocated with split.\n");
        split(result, size);  //split
        return result + sizeof(Node);  //returned result is actual block of memory
    } else {  //result is size only
        printf("Allocated with extra memory.\n");
        result->allocated = 1;  //sets to process - no splitting, allocates entire hole
        return result + sizeof(Node);  //returned result is actual block of memory
    }
}

void *worstFit(size_t size) {
    printf("Attempting to allocate %i bytes.\n", (int) size);
    if ((int) size <= 0) {  //checks if 0 or less than 0
        printf("Error: Can't allocate 0/negative bytes.\n");
        return NULL;
    }
    Node *current = firstN;
    Node *result;  //result node
    Node *nodelessResult;  //result node for allocating with extra memory
    Node *exactResult;  //exact fit result node
    int maximum = 0;  //maximum is smallest value
    int nodelessMaximum = 0;  //maximum for nodeless checking
    int counter = 0;  //counter for suitable hole with extra node - ideal
    int noNode = 0;  //counter for suitable hole without extra node - less ideal
    int exact = 0;  //counter for suitable exact fit - worst case allocation
    while (current != NULL) {  //goes through nodes
        if (current->allocated == 1) {  //if process skip
            current = current->next;
            continue;
        }
        if ((((int) current->size) - (int) (size + sizeof(Node)) > maximum)) {
        //if new gap is biggest including extra node
            result = current;  //result is current node
            maximum = (current->size) - (size + sizeof(Node)); //sets new maximum gap
            counter++;  //increases suitable hole with extra node counter;
        }
        if ((((int) current->size) - ((int) size)) > nodelessMaximum) {
        //if new gap is biggest excluding extra node
            nodelessResult = current;  //nodeless result is current node
            nodelessMaximum = current->size - size; //sets new nodeless maximum gap
            noNode++;  //increases nodeless counter;
        }
        if (current->size == size) {  //if exact fit
            exactResult = current;  //exact result is current
            exact++;  //exact counter increased
        }
        current = current->next;//if current isn't suitable hole, skip
    }

    if (counter > 0) {  //allocating with an extra node takes priority
        printf("Allocated with split.\n");
        split(result, size);  //splits
        return result + sizeof(Node);  //result is actual block of memory
    }
    if (noNode > 0) {  //if this had priority over extra node the entire heap would be allocated to
    //one process no matter how small
        printf("Allocated with extra memory.\n");
        nodelessResult->allocated = 1;  //sets to process
        return result + sizeof(Node);  //result is actual block of memory
    }
    if (exact > 0) {  //worst case before failing - an exact fit
        printf("Allocated with exact fit.\n");
        exactResult->allocated = 1;  //sets to process
        return result + sizeof(Node);  //result is actual block of memory
    } else {  //no suitable hole found
        printf("Error: No space to allocate.\n");
        return NULL;
    }
}




//Test harnesses.
/*
***WARNING***

TEST RESULTS ASSUME 4 BYTE INTEGERS

***WARNING***
*/
void nodePrinter() {
    Node *current = firstN;
    int counter = 1;
    printf("\n");
    while (current != NULL) {
        printf("||Node: %i, Allocated: %i, Size: %i||\n", counter, current->allocated, current->size);
        current = current->next;
        counter++;
    }
    printf("\n");
}

void holeGen(int selection) {   //generates a heap with three holes, 50, 70 and 120
    printf("Initialising test heap \n");
    int *ptr = (int *) malloc(420);
    if (selection == 1) {
        initialise(ptr, 420, "FirstFit");
    } else if (selection == 2) {
        initialise(ptr, 420, "NextFit");
    } else if (selection == 3) {
        initialise(ptr, 420, "BestFit");
    } else if (selection == 4) {
        initialise(ptr, 420, "WorstFit");
    } else {
        printf("Defaulting to FirstFit\n");
        initialise(ptr, 420, "FirstFit");
    }
    printf("State of initial heap:\n");
    nodePrinter();
    void *hole1 = firstFit(50);
    firstFit(10);
    void *hole2 = firstFit(70);
    firstFit(10);
    void *hole3 = firstFit(120);
    deallocate(hole1);
    deallocate(hole2);
    deallocate(hole3);
    printf("State of heap at start of testing:\n");
    nodePrinter();

}

void repeatTest() { //allocates a bunch then deallocates at random. MAKE SURE THERES ENOUGH MEMORY IN INITIALISE
    printf("\n\nREPEAT TESTING \n");
    int *ptr = (int *) malloc(3000);
    initialise(ptr, 3000, "WorstFit");
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

void firstFitTest() {
    holeGen(1);     //generates test heap with holes 50, 70, 120
    printf("\n\nFIRST FIT TESTING \n");
    allocate(500);  //should fail
    allocate(500);  //should fail
    allocate(0);    //should fail
    allocate(-1);   //should fail
    allocate(50);   //exact fit test
    nodePrinter();
    allocate(69);   //should allocate with extra memory
    nodePrinter();
    allocate(69);   //should allocate with split
    nodePrinter();
}

void nextFitTest() {
    holeGen(2);     //generates test heap with holes 50, 70, 120
    printf("\n\nNEXT FIT TESTING \n");
    allocate(500);  //should fail but change trackN
    allocate(500);  //should fail but change trackN
    allocate(0);    //should fail
    allocate(-1);   //should fail
    allocate(50);   //should allocate in 70 byte hole
    nodePrinter();
    allocate(69);   //should allocate in 120 byte hole

    nodePrinter();
}

void bestFitTest() {
    holeGen(3);     //generates test heap with holes 50, 70, 120
    printf("\n\nBEST FIT TESTING \n");
    allocate(500);  //should fail
    allocate(500);  //should fail
    allocate(0);    //should fail
    allocate(-1);   //should fail
    allocate(60);   //should go to hole 70 and allocate with extra memory
    nodePrinter();
    allocate(50);   //should allocate exactly to hole 50

    nodePrinter();
}

void worstFitTest() {
    holeGen(4);     //generates test heap with holes 50, 70, 120
    printf("\n\nWORST FIT TESTING \n");
    allocate(500);  //should fail
    allocate(500);  //should fail
    allocate(0);    //should fail
    allocate(-1);   //should fail
    allocate(50);   //should go into largest hole
    nodePrinter();
    allocate(10);   //should go into new largest hole
    nodePrinter();
    allocate(69);   //should fail since heap is fragmented
    nodePrinter();
    allocate(50);   //should fit exactly
    nodePrinter();
    allocate(10);   //should allocate with extra memory
    nodePrinter();
}




int main() {

    //repeatTest();
    firstFitTest();
    //nextFitTest();
    //bestFitTest();
    //worstFitTest();
    printf("\n\nTESTING FINISHED. \n");

}