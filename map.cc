#include <fstream>
#include <iostream>
#include <string>
#include <stack>
using namespace std;

//maze will use dots to indicate dead ends or changes in direction.
//replace data with node that contains boolean intersection or no intersection. Also contains what directions it can go, as well as if it's already gone those ways.
//program will count what nodes it has visited, when branch forces a dead end it will return back to the last numbered node with intersections available (found through a list search). When it goes through the next branch it overwrites all the numbers it already counted.


//Nodes that let you go left, right or straight. Also can traverse the tree backwards
struct Node{
    string instruction;
    string data;//stored direction
    int directionCount;
    bool directionsLeft;
    bool isIntersection;
    
    Node * left;
    Node * right;
    Node * straight;
    Node * previous;
};

//function to output everything mapped so far
void inorder(Node * root);

//Calculates the size of the graph
int size(Node * root);

//adds a node to the graph
Node* add(Node *& root, Node *& previousNode, string chosenDirection, string line);

//looks to see if the node is an intersection
bool isIntersection(Node * root);

//chooses the correct default direction
string chooseDirection(string line, int space1loc, int numDirections, Node* intersection);

//counts the number of directions given to the computer
int numDirectionsCount(string line, int &space1loc, int &space2loc);

//adds to backtracking
void addToBacktracking(stack<string> &backtracking, string direction);

//outputs backtracking list
void printBacktracking(stack<string> backtracking);

//backtracks up the list
void backtrack(Node* root, Node* cursor, stack<string> &backtracking);

//Moves through the maze with a cursor over the nodes
void moveThroughMaze(Node* &graph, Node* placeholder, stack<string> &backtracking, stack<Node*> &intersections);

//Prints out all the intersections stored
void printIntersections(Node* graph, stack<Node*> intersections);

//Copies the node to the node stack
void copyNode(Node* graph, Node* &intersection);

int main(int argc, char *argv[]){
    
    //Node system for the graph to be stored in
    Node* graph;
    graph = NULL;
    Node* placeholder;
    placeholder = NULL;
    
    //Node system for backtracking function
    stack<string> backtracking;
    //Node system for remembering intersections
    stack<Node*> intersections;
    
    
    ifstream instream;
    instream.open("input.txt");
    
    //Moves through the maze
    moveThroughMaze(graph, placeholder, backtracking, intersections);
    
    cout << endl;
    inorder(graph);
    //output the backtracking list
    printBacktracking(backtracking);
    printIntersections(graph, intersections);
//    cout << endl << "size " << size(graph) << endl;
    
    
    return 0;
}

void inorder(Node* root){
    if(root != NULL){
        cout<<root->data<<endl;
        inorder(root->left);
        inorder(root->right);
        inorder(root->straight);
    }
}

int size(Node * root){
    if(root == NULL) return 0;
    else
    return size(root->left) + size(root->right) + size(root->straight) + 1;
}

Node* add(Node *& root, Node *& previousNode, string chosenDirection, string line){
    if(root == NULL){
        root=new Node;
        root->previous = previousNode;
        root->instruction = line;
        root->data = chosenDirection;
        root->left=root->right=root->straight=root->previous=NULL;
        root->directionCount = 0;
        root->directionsLeft = 0;
        
        return previousNode;
    }
    else if (chosenDirection == "LEFT")
        return add(root->left, root, chosenDirection, line);
    else if (chosenDirection == "STRAIGHT")
        return add(root->straight, root, chosenDirection, line);
    else
        return add(root->right, root, chosenDirection, line);
}

string chooseDirection(string line, int space1loc, int numDirections, Node* intersection){
    if(intersection == NULL){//none of the paths in front of the bot have ever been visited before
        if (line.substr(0, space1loc) == "L"){
            return "LEFT";
        }
        if (line.substr(0, space1loc) == "S"){
            return "STRAIGHT";
        }
        if(line.substr(0, space1loc) == "D"){
            return "DEADEND";
        }
        else
            return "RIGHT";
    }
    else{//one or more paths have already been visited before, came from backtracking
        if(numDirections > 1){
            //left doesn't need to be tried again, check for straight and then maybe right
            int Slocation = line.find("S");
            int Rlocation = line.find("R");
            if((intersection->straight != NULL) && (Slocation != -1)){//if straight is an option in the string AND it has not been visited before
                return "STRAIGHT";
            }
            else if((intersection->right != NULL) && (Rlocation != -1)){
                return "RIGHT";
            }
            else{
                return "AGAIN";//all the pathways at the intersection have been visited, backtracking needs to be applied again
            }
        }
        else{
            return "AGAIN";//signal to loop the backtracking again
        }
    }
}

int numDirectionsCount(string line, int &space1loc, int &space2loc){
    space1loc = line.find(' ');
    space2loc = line.find(' ', space1loc+1);
//    cout << "space1loc: " << space1loc << endl;
//    cout << "space2loc: " << space2loc << endl;
    
    if(space1loc == -1)
        return 1;
    else if(space2loc != -1)
        return 3;
    else
        return 2;
}

void addToBacktracking(stack<string> &backtracking, string chosenDirection){
    if(chosenDirection == "LEFT"){
        backtracking.push("RIGHT");
    }
    else if(chosenDirection == "STRAIGHT"){
        backtracking.push("STRAIGHT");
    }
    else{
        backtracking.push("LEFT");
    }
}

void printBacktracking(stack<string> backtracking){
    cout << endl;
    while (!backtracking.empty()) {
        cout << backtracking.top() <<" ";
        backtracking.pop();
       }
}

void backtrack(Node* root, Node* cursor, stack<string> &backtracking){
    cout << "I'm telling the drive system to turn around. Here is the backtracking list right now:";
    printBacktracking(backtracking);
    
    
    cout << "I'm telling the drive system to move " << cout << backtracking.top() << " Here is the backtracking list right now:";
    backtracking.pop();
    printBacktracking(backtracking);
    
}

bool isIntersection(Node * root){
    int directionCount = 0;
    if(root->left != NULL){
        directionCount++;
    }
    if(root->straight != NULL){
        directionCount++;
    }
    if(root->right != NULL){
        directionCount++;
    }
    
    if(directionCount > 1){
        return 1;
    }
    else{
        return 0;
    }
}

void moveThroughMaze(Node* &graph, Node* placeholder, stack<string> &backtracking, stack<Node*> &intersections){
    int space1loc = -1;
    int space2loc = -1;
    int numDirections = 0;
    string chosenDirection;
    string line;
    
    Node* cursor;
    cursor = graph;
    intersections.push(cursor);//starts the intersection list at the root
    
    
    
    
    
    
    
    
    
    
    
    do{
        numDirections = 0;//reinitializes it every time
        cout << endl << "I'm telling the drive system to go straight for now. What's in front of me?" << endl;
        getline(cin, line);
        
        numDirections = numDirectionsCount(line, space1loc, space2loc);
        if(line == "Q") {
            break;
        }
        
        
        chosenDirection = chooseDirection(line, space1loc, numDirections, NULL);
        cout << "The chosen direction is: " << chosenDirection << endl;
        //if the chosen direction is dead end, DO NOT ADD. BACKTRACK
        if(chosenDirection == "DEADEND"){
            backtrack(graph, cursor, backtracking);
            chosenDirection = chooseDirection(line, space1loc, numDirections);
            //if chosenDirection is again, loop again
            //else go down the free pathway chosenDirection decides
            
            //add()
            
        }
        
        if(numDirections > 1){
            Node* intersection = add(graph, placeholder, chosenDirection, line);
            copyNode(graph, intersection);
            intersections.push(intersection);
        }
        //if numDirectionsCount > 1 cursor = graph along with all its options
        //if chosendirection == DEADEND
        //  backtrack backtracking stack
        //  if the cursor directions are not already all visited
        //      add(cursor, placeholder, chosenDirection)
        //  else pop the stack of cursors, then add(cursor....)
        
        //this needs to be in an else. It only adds if it's done backtracking and there are no more directions.
        
        //CHOSEN DIRECTION NEEDS TO BE UPDATED TO IF THE NODE HAS BEEN VISITED BEFORE
        
        add(graph, placeholder, chosenDirection, line);

        addToBacktracking(backtracking, chosenDirection);
        //    }while(!instream.eof());
    }while(line !="q");
}

void printIntersections(Node* graph, stack<Node*> intersections){
//    cout << "graph -> left:" << graph->left << ":" << endl;
//    cout << "graph -> straight:" << graph->straight << ":" << endl;
//    cout << "graph -> right:" << graph->right << ":" << endl;
    cout << endl;
//    cout << "outside the loop" << endl;
    while (!intersections.empty()) {
//        cout << "inside the loop" << endl;
        cout << "intersection -> left:" << intersections.top()->left << ":" << endl;
        cout << "intersection -> straight:" << intersections.top()->straight << ":" << endl;
        cout << "intersection -> right:" << intersections.top()->right << ":" << endl;
        intersections.pop();
       }
}

void copyNode(Node* graph, Node* &intersection){
    intersection->instruction = graph->instruction;
    intersection->data = graph->data;
    intersection->directionCount = graph->directionCount;
    intersection->directionsLeft = graph->directionsLeft;
    intersection->isIntersection = graph->isIntersection;
    intersection->left = graph->left;
    intersection->right = graph->right;
    intersection->straight = graph->straight;
    intersection->previous = graph->previous;
}

//The copyNode() function needs to copy intersections to the stack, but it also has to copy the intersection back when a new link is made. If it is copied back right after add that should mend the link

//Make sure the instruction count is set back to 0 every time it's supposed to
