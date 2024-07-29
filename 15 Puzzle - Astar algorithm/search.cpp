#include <iostream>                                             
#include <queue>                                                
#include <vector>
#include <set>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <sys/resource.h>
#include <cmath>
using namespace std;
using namespace std::chrono;                                             


// Construct the board, size, value and path
// Tack the initial coordinates to find the possible direction to moves (child)
class node {       
    public:
        vector<vector<int> > board;             
        int xCrd;
        int yCrd;
        string path;
        int size = 0;
        int value = 0;
};


// Class implements the operator overloading
class orderBy {
public:
    bool operator()(const node x, const node y) const {
        return x.value > y.value ? true : false;
    }
};


// Global virables used throughout the project
int expand = 0;
int moves [4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};          // Left, right, up, down
vector<vector<int> > completePuzzle {{1, 2, 3, 4}, 
                                    {5, 6, 7, 8}, 
                                    {9, 10, 11, 12}, 
                                    {13, 14, 15, 0} };          // Completed puzzle


// Return true if puzzle solved or equal to completePuzzle
bool isGoalState (node puzzle) {                         
    if (puzzle.board == completePuzzle) {  
        return 1;
    }
    return 0;
}


// Return true if states hasn't visited already
bool isCycle (set <vector<vector<int> > > visitedStates, node states) { 
    if (visitedStates.find(states.board) != visitedStates.end()) {
        return true;
    }
    return false;
}


// Check if coordinates are in bound for x y (between 0 to 3)
bool isvalid (int y, int x) {          
    if (x >= 0 && x <= 3) { 
        if (y >= 0 && y <= 3) {
            return 1;
        }
    } 
    return 0;
}


// Return move direction as a char (U, D, R, L)
char findPath (int y, int x) { 
    if (y == -1 && x == 0) {                                    // Up
        return 'U';
    }
    else if (y == 1 && x == 0) {                                // Down
        return 'D';
    }
    else if (y == 0 && x == 1) {                                // Right
        return 'R';
    }
    return 'L';                                                 // Left
}


// Calculate the manhattan distance
int run_bfs_manhattan_distance (node puzzle) {
    int total = 0, number = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            number = puzzle.board[i][j];
            if (number == 0) {
                continue;
            }
            
            int i1 = (number - 1) / 4;
            int j1 = (number - 1) % 4;
            total += (abs(i - i1) + abs(j - j1));
        }
    }
    return total;
}


// Calculate the misplaced tile
int run_bfs_misplaced_tile (node puzzle) {
    int tile = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (completePuzzle[i][j] != puzzle.board[i][j]) {
                tile++;
            }
        }
    }   
    return tile;
}
  

// Call the function and calculate the value depeinding the heuristicType
int heuristic (node puzzle, string heuristicType) {
    if (heuristicType == "manhattan") {
        return run_bfs_manhattan_distance (puzzle);
    } 
    else {
        return run_bfs_misplaced_tile (puzzle);
    }
}

  
// Implimenting a star algorithm to solve the 15 puzzle
// print the path and return value from heuristic if puzzle solved
int aStar (node puzzle, string heuristicType) {
    priority_queue < node , vector<node> , orderBy > states;    // Stores the states            
    set <vector<vector<int> > > visitedStates;                  // declared set to stored the visited states
    int result = puzzle.size + heuristic(puzzle, heuristicType);
    states.push(puzzle);                                    
    visitedStates.insert(puzzle.board);
    
    while (!states.empty()) {
        node newNode = states.top();                            // store the node from priority_queue                     
        states.pop();                                              
        int x1 = newNode.xCrd;
        int y1 = newNode.yCrd;
        
        // If puzzle sloved, print the moves and return value from heuristic 
        if (isGoalState(newNode)) {                             
            cout << "Moves: " << newNode.path << endl;          // Print path to reach solve state
            return heuristic (newNode, heuristicType) + newNode.size;
        }

        // explore the possible moves (L, R, U, D) and create a new node and swap the number 
        // push the node to the list and set if haven't visited already 
        for (int i = 0; i < 4; i++) {
            if (isvalid(y1 + moves[i][0], x1 + moves[i][1])) {  // check if coordinates in bound
                node child = newNode;
                int x2 = child.xCrd = x1 + moves[i][1];         // Set values to child class 
                int y2 = child.yCrd = y1 + moves[i][0];
                child.board[y1][x1] = child.board[y2][x2];      // swap the initial coordinates
                child.board[y2][x2] = 0;
                child.path = newNode.path + findPath(moves[i][0], moves[i][1]);
                child.size = newNode.size + 1;
                child.value = child.size + heuristic (child, heuristicType);
                if (!isCycle(visitedStates, child)) {           // insert child to stack and set if not visited 
                    int result = newNode.size + heuristic(newNode, heuristicType);
                    states.push(child);
                    visitedStates.insert(child.board);
                }
            }
        }
        expand++;                                               // increment expanded node
    }
    return 0;                                            
}


// print the out for the game (move, # of node, time, and memory)
// Use high_resolution_clock to record the time in microseconds
void print (node puzzle, string heuristicType) {
    struct rusage usage;
    auto startTime = high_resolution_clock::now(); 
    getrusage(RUSAGE_SELF, &usage);                             // getrusage to find memory used 
    int final = aStar (puzzle, heuristicType);                  // call the aStar function
    auto stopTime = high_resolution_clock::now();           
    auto finalTime = duration_cast<microseconds>(stopTime - startTime);  

    // print required output to console
    cout << heuristicType << ": " << final << endl;             // Print value for the aStar function 
    cout << "Number of Nodes expanded: " << expand << endl;
    cout << "Time Taken: " << finalTime.count() << endl;         
    cout << "Memory Used: " << usage.ru_maxrss << " KB" << endl; 
}

int main () {
    
    node puzzle;                                                // create a puzzle of node class
    string input, number;
    getline(cin, input);                                        // take the input puzzle from user
    stringstream ss(input);                                     // store input stringstream for parsing
    vector<vector<int> > board (4, vector<int>(4, 0));
    int x = 0, y = 0;

    // add input to the board using stringstream
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (ss >> number) {                                 // set up the board
                board[i][j] = stoi (number);
            }
            if (board[i][j] == 0) {                             // initialize initial coordinates
                x = j;
                y = i;
            }
        }
    }
    puzzle.board = board;                                       // store the board in the puzzle class
    puzzle.xCrd = x;
    puzzle.yCrd = y;
    print (puzzle, "manhattan");                                // call print function with manhattan
    cout << endl << endl;
    expand = 0;                                                 // reinitalize the exapant to 0
    print (puzzle, "misplaced");                                // call print function with manhattan
    return 0;
}
