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
// ================== INTERFACE ================== 
// function that draws the map on the console screen
void display(Maze *maze, Backpack *b, int step) {
    system(CLEAR); // clears the screen 
    printf("=== BACKTRACKING SEARCH ===\n");
    printf("Step: %d | Total: %d | Items: %d\n\n", step, b->total_value, b->size);

    // loops through row by row, column by column, printing the characters
    for (int r = 0; r < maze->rows; r++) {
        for (int c = 0; c < maze->cols; c++) {
            printf("%c", maze->display[get_index(maze, r, c)]);
        }
        printf("\n"); 
    }
}

// ================== LOAD ================== 
// this function reads the .txt file that contains the map layout
int load(Maze *maze, char *filename) {
    FILE *f = fopen(filename, "r"); // opens the file in read mode
    if (!f) return 0; // if the file is not found, returns an error

    // the txt file must start with the dimensions, e.g., 10x10. 
    if (fscanf(f, "%dx%d", &maze->cols, &maze->rows) != 2) {
        fseek(f, 0, SEEK_SET); // goes back to the beginning of the file
        if (fscanf(f, "%dX%d", &maze->cols, &maze->rows) != 2) { // tries with a capital X
            fclose(f);
            return 0; // if both fail, gives up
        }
    }

    // skips the rest of the first line in the file until it actually reaches the map drawing
    int ch;
    while ((ch = fgetc(f)) != '\n' && ch != EOF);

    // reads the map row by row
    for (int i = 0; i < maze->rows; i++) {
        char line[100];
        if (fgets(line, sizeof(line), f)) { // reads the text line from the file
            for (int j = 0; j < maze->cols; j++) {
                char c = line[j];
                
                // ignores line breaks that come along in the text file
                if (c == '\n' || c == '\r' || c == '\0') c = ' '; 
                
                int current = get_index(maze, i, j); // calculates the 1D position
                maze->cells[current] = c;            // saves it in the static map
                maze->display[current] = c;          // saves it in the visual map

                if (c == 'P') maze->start = current; // if it finds 'P', notes that it's the starting position
                if (c == 'S') maze->exit = current;  // if it finds 'S', notes that it's the exit of the maze
            }
        }
    }

    fclose(f); 
    return 1;  
}

// ================== SEARCH ================== 
// DFS + backtracking
int search (Maze *maze, Backpack *b){
    int explored [MAX_CELLS] = {0};

    // Direction vectors: Up (-1,0), Down (1,0), Left (0,-1), Right (0,1)
    // Used to calculate the coordinates of the adjacent cells.
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    Stack stack = {NULL};       // initializes an empty stack of steps
    int pos = maze->start;      // the explorer starts at position 'P'
    explored[pos] = 1;          // marks the starting point as visited
    int step = 0;               // step counter for the UI interface

    // only stops when finding the exit or realizing it's impossible.
    while (1) {
        // NOTE: make sure you translate the 'exibir' function to 'display' later!
        display(maze, b, step++); // draws the screen
        SLEEP(DELAY_MS);          // pause for the animation

        if (pos == maze->exit) return 1; // VICTORY CONDITION: found the 'S'

        int moved = 0; // flag to know if we managed to move somewhere

        // tries to look at the 4 surrounding directions (up, down, left, right)
        for (int d = 0; d < 4; d++) {
            // calculates the row (nr) and column (nc) of the neighbor we are looking at
            int nr = (pos / maze->cols) + dr[d];
            int nc = (pos % maze->cols) + dc[d];

            // if the neighbor is outside the map boundaries, ignore and try another direction
            if (nr < 0 || nc < 0 || nr >= maze->rows || nc >= maze->cols)
                continue;

            int p = get_index(maze, nr, nc); // gets the 1D position of this neighbor

            // if the neighbor is a Wall ('#') or we have already visited it, ignore
            if (maze->cells[p] == '#' || explored[p]) continue;

            // if we reached here, we found a valid path!
            StepRecord record = {pos, 0}; // creates the record for the stack (where I am right now)

            
            if (maze->cells[p] == 'T') { 
                // if stepped on a Treasure ('T')
                int v = rand() % 100 + 1; // generates a random value from 1 to 100
                add_backpack(b, v);       // stores it in the backpack
                record.treasure = v;      // notes in the stack that we picked up this treasure here
            } else if (maze->cells[p] == 'A') {
                // if stepped on a Trap ('A'), lose the least valuable treasure
                remove_first_treasure(b);
            }

            push(&stack, record); // saves in the stack that we were here

            maze->display[pos] = '.'; // leaves a breadcrumb on the old position
            pos = p;                  // moves the explorer to the new square
            explored[p] = 1;           // marks that we stepped on it so we don't return aimlessly
            maze->display[p] = 'P';   // draws the explorer ('P') in the new square

            moved = 1; // signals that we successfully moved
            break;     // since we found a path, break the direction-checking loop and walk
        }
        
        // BACKTRACKING: if we tested all 4 directions and couldn't move anywhere (dead end)
        if (!moved) {
            if (is_empty(&stack)) return 0; // if the stack is empty, maze has no solution!

            maze->display[pos] = '.';       // leaves a breadcrumb in the dead end
            StepRecord record = pop(&stack);  // pops the last step from the stack 

            // If we picked up a treasure in the exact place we are now undoing, we must return this treasure 
            if (record.treasure > 0) {
                // this block searches the linked list to specifically delete 
                // the treasure that matches the exact value we picked up
                Treasure *prev = NULL, *cur = b->start;
                
                // runs through the list until it finds the exact treasure
                while (cur && cur->value != record.treasure) {
                    prev = cur;
                    cur = cur->next;
                }
                
                // if the treasure is found, rewires the connections to exclude it from the list
                if (cur) {
                    if (prev) prev->next = cur->next; // if it's in the middle/end
                    else b->start = cur->next;        // if it was the first item

                    b->total_value -= cur->value;
                    b->size--;
                    free(cur); // frees the memory of the returned treasure
                }
            }

            pos = record.pos;             // moves the explorer to the previous position
            maze->display[pos] = 'P';     // draws the explorer there
        }
    
    }
}

int main() {
    srand(time(NULL)); // initializes the random number generator seed using the computer's current time
    Maze maze;         // creates the map variable
    Backpack b;        // creates the backpack variable
    backpack_init(&b); // initializes the backpack as empty

    // tries to load the txt file containing the map layout.
    if (!load(&maze, "maze.txt")) {
        printf("Error opening maze.txt\n");
        return 1; // exits with an error code if the file is not found
    }

    // triggers the search function
    if (search(&maze, &b)) { // if the function returns 1 (True/Found the exit)...
        printf("\nEXIT FOUND!\nTotal Coins: %d\n", b.total_value);
    } else {                 // if it returns 0 (Stack emptied and didn't find the exit)
        printf("\nDEFEAT! The maze has no reachable exit.\n");
    }

    return 0; // returns 0, informing the Operating System that the program finished successfully
}