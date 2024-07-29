#include <iostream>                                             // C++ libraries
#include <list>                                                 // data stucture libraries
#include <vector>
#include <set>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <sys/resource.h>
using namespace std;
using namespace std::chrono;

vector<vector<int> > complete_puzzle {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 0}}; // completed puzzle
int moves [4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};          // left, right, up, down
int expand = 0;                                                 // count expanded nodes

// construct the board for each state and record the possible moves
// trach the initial coordinates to find the possible direction to moves (childStates)
struct node {                                       
    vector<vector<int> > board;             
    int Xcoordinate;
    int Ycoordinate;
    string recordStates;
};

bool isGoalState (node puzzle) {                                // check if puzzle solved (equal to complete_puzzle)
    if (puzzle.board == complete_puzzle) {  
        return 1;
    }
    return 0;
}

bool visiteNode (set <vector<vector<int> > > visitedStates, node states) {   // check if states already visited
    if (visitedStates.find(states.board) != visitedStates.end()) {
        return 1;
    }
    return 0;
}

bool isvalid (int xCoordinate, int ycoordinate) {               // check if coordinates are in bound (0, 3)
    if (xCoordinate >= 0 && xCoordinate <= 3) { 
        if (ycoordinate >= 0 && ycoordinate <= 3) {
            return 1;
        }
    } 
    return 0;
}

char findAction (int y, int x) {                               // return the move direction U, D, R, L
    if (y == -1 && x == 0) {
        return 'U';
    }
    else if (y == 1 && x == 0) {
        return 'D';
    }
    else if (y == 0 && x == 1) {
        return 'R';
    }
    return 'L';
}

node breadthFirstSearch (node puzzle) {
    list<node> states;                                          // declared list to push and pop the states
    set <vector<vector<int> > > visitedStates;                  // declared set to stored the visited states
    states.push_back(puzzle);                                   // push initial puzzle to the list
    
    while (!states.empty()) {
        node newNode = states.front();  
        if (isGoalState(newNode)) {                             // return node if the puzzle sloved 
            expand++;
            return newNode;
        }
        if (!visiteNode(visitedStates, newNode)) {              // add board to set if states haven't visited
            visitedStates.insert(newNode.board);
        }
        // explore the possible moves (L, R, U, D) and create a new node and swap the number 
        // push the node to the list and set if haven't visited already 
        for (int i = 0; i < 4; i++) {
            if (isvalid(newNode.Xcoordinate + moves[i][1], newNode.Ycoordinate + moves[i][0])) {    // check if coordinates in bound
                node childStates = newNode;
                childStates.Xcoordinate = newNode.Xcoordinate + moves[i][1];    // store new coordinates 
                childStates.Ycoordinate = newNode.Ycoordinate + moves[i][0];
                childStates.board[newNode.Ycoordinate][newNode.Xcoordinate] = childStates.board[childStates.Ycoordinate][childStates.Xcoordinate];
                childStates.board[childStates.Ycoordinate][childStates.Xcoordinate] = 0;
                char action = findAction (moves[i][0], moves[i][1]);
                childStates.recordStates.push_back(action);
                if (!visiteNode(visitedStates, childStates)) {      // push the node to states list and visitedStates set 
                    states.push_back(childStates);
                    visitedStates.insert(childStates.board);
                }
            }
        }
        states.pop_front();                                     // pop the states after exploring 
        expand++;                                               // increment expanded node
    }
    node failure;
    return failure;                                             // return failure for unsolvable puzzle
}

void print (node puzzle) {
    struct rusage usage;
    auto startTime = high_resolution_clock::now(); 
    getrusage(RUSAGE_SELF, &usage);
    node finalPuzzle = breadthFirstSearch (puzzle);             // call the breadthFirstSearch function
    auto stopTime = high_resolution_clock::now();
    auto finalTime = duration_cast<microseconds>(stopTime - startTime);  // record time taken by function in microseconds 

    // print required output to console
    cout << "Moves: " << finalPuzzle.recordStates << endl;
    cout << "Number of Nodes expanded: " << expand << endl;
    cout << "Time Taken: " << finalTime.count() << endl;
    cout << "Memory Used: " << usage.ru_maxrss << " KB" << endl;
}

int main () {
    
    node puzzle;                                                // create a puzzle of node stuct
    string input, number;
    getline(cin, input);                                        // take the input puzzle from user
    stringstream ss(input);                                     // store input stringstream for parsing
    vector<vector<int> > board (4, vector<int>(4, 0));
    int x = 0, y = 0;

    // add input puzzle into a board using stringstream
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (ss >> number) {                                 // set up the board
                board[i][j] = stoi (number);
            }
            if (board[i][j] == 0) {                            // initialize initial coordinates
                x = j;
                y = i;
            }
        }
    }
    puzzle.board = board;
    puzzle.Xcoordinate = x;
    puzzle.Ycoordinate = y;

    print (puzzle);                                                // call print function
    
    return 0;
}
