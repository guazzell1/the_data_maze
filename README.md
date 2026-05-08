# The Data Maze

A terminal-based maze exploration game built in C, developed as an academic project for the Data Structures course at Facens.

The goal: navigate a treasure hunter through an ancient labyrinth, collect as much value as possible, and find the exit — without losing everything to traps.

## 🎮 How It Works
The maze is loaded from a `.txt` file and rendered in the terminal with ASCII characters. A Depth-First Search + Backtracking algorithm automatically navigates the player through the maze, step by step, with an animated delay so you can watch the exploration in real time.

### Map Legend
| Symbol | Meaning |
| :---: | :--- |
| `#` | Wall |
| ` ` | Open corridor |
| `P` | Player (start) |
| `S` | Exit |
| `T` | Treasure |
| `A` | Trap |

---

## 🧠 Data Structures Used

### 🎒 Backpack — Sorted Linked List
Each treasure found is stored in a sorted linked list (ascending order by value)[. This ensures the least valuable item is always at the front — minimizing losses when a trap is triggered.
* **Trap rule:** Stepping on `A` removes the first item (lowest value).
* **Backtracking rule:** If the player undoes a step where a treasure was collected, that treasure is removed from the backpack.

### 🔁 Stack — DFS Memory
A linked-list stack stores the history of each step taken. Each record saves:
1. The position the player came from.
2. The treasure value picked up at that step (if any).

When a dead end is reached, the algorithm pops the stack and backtracks — undoing treasure pickups as needed to maintain consistency.

---

## 🔍 Algorithm: DFS + Backtracking
1. Start at `P`
2. Try each direction (Up, Down, Left, Right)
3. If a valid unvisited cell is found, push current position to stack and move
4. If all directions are blocked → pop from stack and backtrack
5. Repeat until `S` is reached or the stack empties (no solution)

*Visited cells are tracked with a boolean array to prevent infinite loops.*

---

## 📁 File Structure
```text
the_data_maze/
├── the_data_maze.c   # Main source code
├── the_data_maze     # Compiled binary (Linux)
└── maze.txt          # Maze layout file
```

---

## ▶️ How to Run

### Compile
```bash
gcc the_data_maze.c -o the_data_maze
```

### Run
```bash
./the_data_maze
```

> Make sure `maze.txt` is in the same directory as the executable.

### Windows
The code supports both Linux and Windows via preprocessor directives (`Sleep` vs `usleep`, `cls` vs `clear`).

---

## 📄 Maze File Format

The first line must declare the dimensions (`COLSxROWS`), followed by the map:

```
20x20
####################
#P    #    T       #
# ### # ###### ### #
# #        #     # #
# # ###### # ##### #
# # #    T # #   A #
#   # #### # # ### #
# ### #    # #   # #
#     # #### ### # #
####### #  A   # # #
#T      ###### # # #
# #####      # # # #
# #   ######## # # #
# # A #   T    #   #
# ##### ###### #####
#     # #    #     #
### # # # ## ##### #
#   # # T  # #S    #
# ### ###### # ### #
#            #     #
####################
```
The program supports mazes up to **40×40**.

---

## 📊 Output

At each step, the terminal displays:
- The current maze state with the player position (`P`) and trail (`.`)
- Step count, total coin value, and number of items in the backpack

At the end:
- **Victory:** Total coins collected printed to the console
- **Defeat:** Message indicating no reachable exit

The solution path is left as a `.` trail on the final map display.

---

## 🛠️ Technical Details

- **Language:** C (C99 compatible)
- **No external libraries** — pure standard C (`stdio`, `stdlib`, `string`, `time`)
- **Cross-platform:** Linux & Windows
- **Memory management:** All nodes (stack, backpack) allocated with `malloc` and freed properly

---

## 👤 Author

**Murilo Guazzelli da Silveira**  
Computer Engineering Student — Facens, Sorocaba/SP  
[github.com/guazzell1](https://github.com/guazzell1)

---

*Academic project — Data Structures course, Prof. Luciano Freire, Prof. Marco Montebello — Facens 2026*
