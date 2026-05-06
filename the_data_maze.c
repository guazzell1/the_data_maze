#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Change the code based on the operating system you are using
#ifdef _WIN32 
    #include <windows.h> 
    #define SLEEP(ms) Sleep(ms) 
    #define CLEAR "cls"         
#else 
    #include <unistd.h>  
    #define SLEEP(ms) usleep((ms) * 1000) 
    #define CLEAR "clear"                 
#endif

#define MAX_ROWS 40
#define MAX_COLS 40
#define MAX_CELLS (MAX_ROWS * MAX_COLS)
#define DELAY_MS 150

//====== Backpack ======

typedef struct Treasure {
    int value;
    struct Treasure *next; // saves the location of the next treasure
} Treasure;

typedef struct Backpack {
    int size;
    int total_value;
    Treasure *start; // points to the first item
} Backpack; 

// initialize the backpack
void backpack_init(Backpack *b){    // without the pointer, the code would produce a temp copy of the backpack, which has no use for us
    b->start = NULL;
    b->size = 0;            // no items 
    b->total_value = 0;     // no value
}

void add_backpack(Backpack *b, int value){  // original backpack + value of the treasure we found
    Treasure *new = malloc(sizeof(Treasure));   // 'new' gets the location of the "space" malloc returned
    new->value = value;     // stores the treasure's value in "value"
    new->next = NULL;       // initial state


    if(
        !b->start   // if the backpack is empty
        ||          //OR
        value < b->start->value     // current treasure is less than first treasure
    ){
        new->next = b->start; // new treasure points to the "older" first treasure/initial state
        b->start = new;       // the start of the backpack now points to the new treasure
    } else {    // if it is not the smallest one, search for the correct placement in the list
        Treasure *cur = b->start; // auxiliary temporary variable

        // runs through the list until it finds correct placement, or until the list is over
        while(
            cur->next   // is there anything next?
            &&
            cur->next->value <= value // is the next treasure's value smaller than or equal to the new one?
        )
            cur = cur->next; // if the answer to both questions is "yes", go to the next treasure
        // fits the new treasure properly in the list
        new->next = cur->next; 
        cur->next = new;
    }
    b->size ++;
    b->total_value += value;
}

// in case we fall into a trap, removes the least valuable treasure from the backpack
int remove_first_treasure (Backpack *b){
    if (!b->start) return -1; // if the backpack is already empty, dont change anything

    Treasure *temp = b->start; // first treasure
    int val = temp->value;     // save its value
    b->start = temp->next;     // backpack now starts from the second treasure
    free(temp);                // free the memory of the removed treasure
    b->size--;                 // reduce the size of the backpack
    b->total_value -= val;     // subtract the thrown away value from the total
    return val;                // returns the value that was thrown away 
}
 
// ================== MAZE ==================
// structure that holds the entire game map
typedef struct {
    char cells[MAX_CELLS];      // The real, static map (walls, original traps, etc.)
    char display[MAX_CELLS];    // The visual map (where we draw the player and the '.' trail)
    int rows, cols;             // Actual dimensions of the maze read from the file
    int start, exit;            // Stores the index positions of Start ('P') and Exit ('S')
} Maze;

// Mathematical function: converts 2D coordinates (row and col) into a 1D array position.
int get_index(Maze *maze, int row, int col) {
    return (row * maze->cols) + col;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Change the code based on the operating system you are using
#ifdef _WIN32 
    #include <windows.h> 
    #define SLEEP(ms) Sleep(ms) 
    #define CLEAR "cls"         
#else 
    #include <unistd.h>  
    #define SLEEP(ms) usleep((ms) * 1000) 
    #define CLEAR "clear"                 
#endif

#define MAX_ROWS 40
#define MAX_COLS 40
#define MAX_CELLS (MAX_ROWS * MAX_COLS)
#define DELAY_MS 150

//================== Backpack ==================

typedef struct Treasure {
    int value;
    struct Treasure *next; // saves the location of the next treasure
} Treasure;

typedef struct Backpack {
    int size;
    int total_value;
    Treasure *start; // points to the first item
} Backpack; 

// initialize the backpack
void backpack_init(Backpack *b){    // without the pointer, the code would produce a temp copy of the backpack, which has no use for us
    b->start = NULL;
    b->size = 0;            // no items 
    b->total_value = 0;     // no value
}

void add_backpack(Backpack *b, int value){  // original backpack + value of the treasure we found
    Treasure *new = malloc(sizeof(Treasure));   // 'new' gets the location of the "space" malloc returned
    new->value = value;     // stores the treasure's value in "value"
    new->next = NULL;       // initial state


    if(
        !b->start   // if the backpack is empty
        ||          //OR
        value < b->start->value     // current treasure is less than first treasure
    ){
        new->next = b->start; // new treasure points to the "older" first treasure/initial state
        b->start = new;       // the start of the backpack now points to the new treasure
    } else {    // if it is not the smallest one, search for the correct placement in the list
        Treasure *cur = b->start; // auxiliary temporary variable

        // runs through the list until it finds correct placement, or until the list is over
        while(
            cur->next   // is there anything next?
            &&
            cur->next->value <= value // is the next treasure's value smaller than or equal to the new one?
        )
            cur = cur->next; // if the answer to both questions is "yes", go to the next treasure
        // fits the new treasure properly in the list
        new->next = cur->next; 
        cur->next = new;
    }
    b->size ++;
    b->total_value += value;
}

// in case we fall into a trap, removes the least valuable treasure from the backpack
int remove_first_treasure (Backpack *b){
    if (!b->start) return -1; // if the backpack is already empty, dont change anything

    Treasure *temp = b->start; // first treasure
    int val = temp->value;     // save its value
    b->start = temp->next;     // backpack now starts from the second treasure
    free(temp);                // free the memory of the removed treasure
    b->size--;                 // reduce the size of the backpack
    b->total_value -= val;     // subtract the thrown away value from the total
    return val;                // returns the value that was thrown away 
}

// ================== MAZE ==================
// structure that holds the entire game map
typedef struct Maze {
    char cells[MAX_CELLS];      // The real, static map (walls, original traps, etc.)
    char display[MAX_CELLS];    // The visual map (where we draw the player and the '.' trail)
    int rows, cols;             // Actual dimensions of the maze read from the file
    int start, exit;            // Stores the index positions of Start ('P') and Exit ('S')
} Maze;

// Mathematical function: converts 2D coordinates (row and col) into a 1D array position.
int get_index(Maze *maze, int row, int col) {
    return (row * maze->cols) + col;
}

// ================== STACK (MEMORY) ================== 
// This structure holds the "memory" of where the explorer has been (for Backtracking)
typedef struct StepRecord {
    int pos;        // position in the map where the player stepped
    int treasure;   // stores the value of the treasure picked up at this exact spot (0 if none)
} StepRecord;

typedef struct StackNode {
    StepRecord record;          // the data of the step mentioned above
    struct StackNode *next;     // points to the item below in the stack
} StackNode;

typedef struct Stack {
    StackNode *top;     // points to the item currently at the top of the stack
} Stack;

void push(Stack *s, StepRecord record){
    StackNode *n = malloc(sizeof(StackNode));
    n->record = record;     // stores position and treasure 
    n->next = s->top;       // links the new node to the current top (putting it below)
    s->top = n;             // updates the stack so the new node is the top
}

// Pop = Remove something from the top of the stack (used when hitting a dead end and needing to backtrack)
StepRecord pop(Stack *s) {
    StackNode *tmp = s->top;       // Grabs the item currently at the top
    StepRecord record = tmp->record; // Saves the information
    s->top = tmp->next;            // The new top becomes the item immediately below
    free(tmp);                     // Deletes the item that was at the top
    return record;                 // Returns the information of where the player was
}

// Checks if the stack of steps is empty
int is_empty(Stack *s) {
    return s->top == NULL;
}