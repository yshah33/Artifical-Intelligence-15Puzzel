#include <iostream>                                             
#include <stack>                                                
#include <vector>
#include <set>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <sys/resource.h>
using namespace std;
using namespace std::chrono;                                             

// construct the board, depth and path
// tack the initial coordinates to find the possible direction to moves (child)
class node {       
    public:
        vector<vector<int> > board;             
        int xCrd;
        int yCrd;
        string path;
        int depth = 0;
};

// global virables used throughout the project
int moves [4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};          // left, right, up, down
int expand = 0;
bool isCutoff = 0, isResult = 0;    
node failure;                                                   // failure is empty

// return true if puzzle solved or equal to completePuzzle
bool isGoalState (node puzzle) {                         
    vector<vector<int> > completePuzzle {{1, 2, 3, 4}, 
                                         {5, 6, 7, 8}, 
                                         {9, 10, 11, 12}, 
                                         {13, 14, 15, 0} };      // completed puzzle
    
    if (puzzle.board == completePuzzle) {  
        return 1;
    }
    return 0;
}

// return 1 if states hasn't visited already
bool isCycle (set <vector<vector<int> > > visitedStates, node states) { 
    if (visitedStates.find(states.board) != visitedStates.end()) {
        return true;
    }
    return false;
}

// check if coordinates are in bound for x y (between 0 to 3)
bool isvalid (int y, int x) {          
    if (x >= 0 && x <= 3) { 
        if (y >= 0 && y <= 3) {
            return 1;
        }
    } 
    return 0;
}

// return move direction as a char (U, D, R, L)
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


node deathFirstSearch (node puzzle, int length) {
    stack<node> states;                                          // declared stack to store states (LIFO)
    set <vector<vector<int> > > visitedStates;                   // declared set to stored the visited states
    states.push(puzzle);                                        
    
    while (!states.empty()) {
        node newNode = states.top();                            // store the last node from stack and explore                     
        states.pop();                                              
        int x1 = newNode.xCrd;
        int y1 = newNode.yCrd;
        if (isGoalState(newNode)) {                             // return 1 and set result node if puzzle sloved 
            expand++;
            isResult = 1;
            return newNode;
        }
        if (newNode.depth > length) {                           // cheack cutoff
            isCutoff = 1;
        }
        else if (!isCycle(visitedStates, newNode)) {            // check if a cycle
            visitedStates.insert(newNode.board);                // insert the node to set if not already
            isCutoff = 0;
                // explore the possible moves (L, R, U, D) and create a new node for each 
                for (int i = 0; i < 4; i++) {
                    // insert or swap the values to node and add child node to stack and set
                    if (isvalid(y1 + moves[i][0], x1 + moves[i][1])) { // check if coordinates in bound
                        node child = newNode;
                        int x2 = child.xCrd = x1 + moves[i][1];  
                        int y2 = child.yCrd = y1 + moves[i][0];
                        child.board[y1][x1] = child.board[y2][x2];
                        child.board[y2][x2] = 0;
                        child.path = newNode.path + findPath(moves[i][0], moves[i][1]);
                        child.depth = newNode.depth + 1;
                        if (!isCycle(visitedStates, child)) {  // insert child to stack and set if not visited 
                            states.push(child);
                        }
                    }
                }
            }
            expand++;                                           // increment expanded node
        }
    return failure;                                             // return failure
}

// return the result node if not cutoff or failure
// call deathFirstSearch() infinitly in while loop until returns true
node deepeningSearch (node puzzle) {
    int index = 0;
    while (1) {
        node result = deathFirstSearch(puzzle, index);
        if (isResult == 1) {
            return result;
        }
        index++;
    }
    return failure;
}

// print the out for the game (move, # of node, time, and memory)
// Use high_resolution_clock to record the time
void print (node puzzle) {
    struct rusage usage;
    auto startTime = high_resolution_clock::now(); 
    getrusage(RUSAGE_SELF, &usage);                             // getrusage to find memory used 
    node finalPuzzle = deepeningSearch (puzzle);                // call the breadthFirstSearch function
    auto stopTime = high_resolution_clock::now();           
    auto finalTime = duration_cast<microseconds>(stopTime - startTime);  

    // print required output to console
    cout << "Moves: " << finalPuzzle.path << endl;              // Print path to reach solve state
    cout << "Number of Nodes expanded: " << expand << endl;
    cout << "Time Taken: " << finalTime.count() << endl;        // time in microseconds     
    cout << "Memory Used: " << usage.ru_maxrss << " KB" << endl; 
}

int main () {
    
    node puzzle;                                               // create a puzzle of node class
    string input, number;
    getline(cin, input);                                       // take the input puzzle from user
    stringstream ss(input);                                    // store input stringstream for parsing
    vector<vector<int> > board (4, vector<int>(4, 0));
    int x = 0, y = 0;

    // add input to the board using stringstream
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (ss >> number) {                                // set up the board
                board[i][j] = stoi (number);
            }
            if (board[i][j] == 0) {                            // initialize initial coordinates
                x = j;
                y = i;
            }
        }
    }
    puzzle.board = board;
    puzzle.xCrd = x;
    puzzle.yCrd = y;
    print (puzzle);                                            // call print function
    
    return 0;
}
