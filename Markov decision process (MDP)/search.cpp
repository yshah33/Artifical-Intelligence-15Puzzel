#include <iostream>                                             
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
using namespace std;

// created a matrix struct to keep tack of the variables at each square on the grid
struct matrix {
    int x;
    int y;
    double val = 0.0;
    double val2 = -1;
    bool isWall = 0;
    int TState = 0;
    char move;
};

// created a coor struct to keep track of the coordinates
struct coor {
    int x; 
    int y;
};

// declared gloabl variables 
int s1 = 0, s2 = 0;
double reward = 0.0, epsilon = 0.0, discount = 0.0;
ofstream output ("output.txt");                                     // write to the file
double T[4];
vector<vector <matrix> > grid;                                      // declared the grid
char path[] = {'S','N','W','E'};
vector<vector <coor>> sides = {                                     // Movement directions
        {{-1, 0}, {0, -1}, {0, 1}, {1, 0}}, 
        {{1, 0}, {0, 1}, {0, -1}, {-1, 0}}, 
        {{0, -1}, {1, 0}, {-1, 0}, {0, 1}}, 
        {{0, 1}, {-1, 0}, {1, 0}, {0, -1}}
    };

// print the value at the grid
void print () {
    // traverse through the grid 
    for (int i = s2 - 1; i >= 0; i--) {
        for (int j = 0; j < s1; j++) {
            // print the value if not a val otherwise print ----
            if (grid[i][j].isWall) {
                output << "-------------- ";
            } 
            else {
                output << grid[i][j].val << " ";
            }
        }
        output << endl;
    }
    output << endl;
    
}

// Calculate the sum for Q-value 
double sum (double &value, int k, int i , int j) {
    // calculate the Qvalue for the non-terminal state
    if (grid[i][j].TState <= 0) {
            value += T[k] * (reward + (discount * grid[i][j].val));
    }
    else {
        // calculate the Qvalue for the terminal state
        value += T[k] * (grid[i][j].TState + (discount * grid[i][j].val));
    }
    return value;
}

// calculate the Q-value for a given state and action
double QValue (int x, int y, int z) {
    double value = 0.0;
     int k = 0;

    // move all four possible direction (up, down, right, left)
    while (k < sides.size()) { 
        int i = 0, j = 0;
        i = x + sides[z][k].x;
        j = y + sides[z][k].y;
        // check i and j are out of bounce or it's wall
        if (i < 0 || i >= s2 || j < 0 || j >= s1 || grid[i][j].isWall) {
            i = x;
            j = y;
        }
        value = sum(value, k, i, j);                    // call sum funciton
        k = k + 1;
    }
    return value;                                       // return the value (double)
}

// calculate the valueIteration and write each iteration to the file
void valueIterator () {
    int i = 0;                                                              // declared the local variables 
    double delta = 0.0, sum = 0.0;
    
    do {
        delta = 0.0;
        output << "iteration: " << i << endl;
        print();

        // traverse through the grid
        for (int i = 0; i < s2; i++) {
            for (int j = 0; j < s1; j++) {
                for (int k = 0; k < 4; k++) {
                    // if the square is not terminal state and the Qvalue is > square's val2,
                    // then change val2 to return value of QValue and update the more
                    if (grid[i][j].TState == 0 && QValue(i, j, k) > grid[i][j].val2) {     
                        grid[i][j].val2 = QValue(i, j, k);
                        grid[i][j].move = path[k];
                    }
                }
                // update the delta new max value
                if (abs(grid[i][j].val - grid[i][j].val2) > delta) {
                    delta = abs(grid[i][j].val - grid[i][j].val2);
                }
            }
        }
        i++;                                     // increment the iterator
        sum = double(epsilon * ((1.0 - discount) / discount));

        // update the val1 to val2 after getting the Qvalue for each square in the iteration
        for (int i = 0; i < s2; i++) {
            for (int j = 0; j < s1; j++) {
                if (grid[i][j].TState == 0) {
                    grid[i][j].val = grid[i][j].val2;
                }
            }
        }
        if (delta <= sum) {                         // break the loop 
            break;
        }
    } while (delta >= sum);
}

// Print the final policy
void printFinal () {
    // traverse through the grid
    for (int i = s2 - 1; i >= 0; i--) {
        for (int j = 0; j < s1; j++) {
            if (grid[i][j].isWall == 1) {                   // print - if wall
                output << "- ";
            }  
            else if(grid[i][j].TState != 0) {               // print T if terminal state
                output << "T ";
            } 
            else {                                          // print the move
                output << grid[i][j].move << " ";   
            }
        }
       output << endl;
    }
}


int main() {
    
    string path, input;                             // local variables
    int temp1, temp2, temp3;
    cout << "Enter file path" << endl;              // take the file path
    cin >> path;
    ifstream file (path);
    
    // dispaly error message if unable to open the file
    if (!file.is_open()) {
        cout << "--------Error opening the file--------" << endl;
        exit(0);
    }
    // read the file until the end of the file 
    while (getline (file, input)) {
        stringstream ss(input);                     // parse the string 
        if (input == "") {
            continue;
        }
        
        string value1, value2, temp;
        ss >> value1 >> value2;
        
        // if value1 is size, the store the size of the grid and resize the grid 
        if (value1 == "size" && value2 == ":") {             
            ss >> s1 >> s2;
            grid.resize(s2, vector<matrix>(s1));
            output << "(" << s1 << ", " << s2 << ", ";
        }
        // if value1 is walls, then update all the wall in the grid
        else if (value1 == "walls" && value2 == ":") {
            output << "[";
            do {
                ss >> temp1 >> temp2;
                grid[temp2-1][temp1-1].isWall = 1;
                if (temp == ",") {
                    output << ", ";
                }
                output << "x= " << temp1 << " y= " << temp2;
                temp = "";
                ss >> temp;
            } while (temp == ",");                  // continou reading the terminal state unitl , is not present
            output << "], ";
        }
        // if value1 is terminal_states, then update all the TStatea and the val2 to 0 in the grid
        else if (value1 == "terminal_states" && value2 == ":") {
            output << "{";
            do {
                ss >> temp1 >> temp2 >> temp3;
                grid[temp2-1][temp1-1].TState = temp3;
                grid[temp2-1][temp1-1].val2 = 0;
                if (temp == ",") {
                    output << ", ";
                }
                output << "x= " << temp1 << " y= " << temp2 << ": " << temp3;
                temp = "";
                ss >> temp;
            } while (temp == ",");                 // continou reading the terminal state unitl , is not present
            output << "}, ";
        }
        // if value1 is reward, then store the reward 
        else if (value1 == "reward" && value2 == ":") {
            ss >> reward;
            output << reward << ", '";
        }
        // if value1 is transition_probabilities, then store the values in the vector 
        else if (value1 == "transition_probabilities" && value2 == ":") {
            ss >> T[0] >> T[1] >> T[2] >> T[3];
            output << T[0] << " " << T[1]  << " " << T[2]  << " " << T[3]  << "', ";
        }
        // if value1 is discount_rate, then store the values 
        else if (value1 == "discount_rate" && value2 == ":") {
            ss >> discount;
            output << discount << ", ";
        }
        // if value1 is epsilon, then store the values 
        else if (value1 == "epsilon" && value2 == ":") {
            ss >> epsilon;
            output << epsilon << ")\n";
        }
    }
    file.close();                                   // close the file
        
    output << "################ VALUE ITERATION ###########################" << endl;
    valueIterator ();                               // call valueIterator()
    
    output << "Final Value After Convergence" << endl;
    print ();                                       // print the grid
    
    output << "Final Policy" << endl;
    printFinal ();                                  // print the final policy
    
    output.close();                                 // close the file
    return 0;
}
