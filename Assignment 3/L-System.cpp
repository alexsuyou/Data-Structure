/***********************************************************************
* Author: Ä¬¯§¼W(I133040010)
* Date: Apr. 6, 2026
* Purpose: Assignment 3 -  L-System
***********************************************************************/

#include <iostream>
#include <fstream> // Used for file writing
#include <cmath>
#include <iomanip>
using namespace std;

#define PI acos(-1.0) // define the const PI

class Lsystem{
  private:
    // 1. General Member
    double curr_x, curr_y, curr_ang; // Current location and angle
    string current_string; // The resulting string after each iterations
    double mov_length, mov_ang; // the angle and the length we want to move and turn
    
    // 2. Stack Member
    // Structure to store the state (like position and angle) for the stack
    struct state{
      double x, y, angle; 
    };
    static const int capacity = 1000; // the capacity of the stack
    state stack[capacity]; // the stack to store the state
    int top = -1; // a poitor point the top of the stack
    
    // 3. Stack Operation
    void PushState(); // push the state in the stack
    void PopState(); // pop the state in the stack
    
    // 3. Rule Member(A -> B)
    char rule_left[10]; // Store the left part of the rule (A)
    string rule_right[10]; // Store the right part of the rule (B)
    int rule_num; // the number of the rule_num
    
  public:
    Lsystem(double length, double ang); // Constructor
    ~Lsystem(){}; // Destructor
    void setAxion(string axiom); // set the axiom
    void addRule(char left, string right); // Adding Rule into the rule_left and rule_right array
    void generation(int iterations); // generate the resulting string 
    void draw(string filename); // Output the result of the movement
};

void Lsystem::PushState(){
  if(top == capacity - 1){ // if the stack is full
    throw "The stack is full.";
  }else{ // otherwise
    top++;
    stack[top].x = curr_x;
    stack[top].y = curr_y;
    stack[top].angle = curr_ang;
  }
}

void Lsystem::PopState(){
  if(top == -1){ // if the stack is empty
    throw "The stack is empty.";
  }else{ // otherwise
    curr_x = stack[top].x;
    curr_y = stack[top].y;
    curr_ang = stack[top].angle;
    top--;
  }
}


Lsystem::Lsystem(double length, double ang):mov_length(length), mov_ang(ang)
{
  // Set the default value
  curr_x = 0;
  curr_y = 0;
  curr_ang = 0;
  top = -1; // default top value is -1
  rule_num = 0;
}

void Lsystem::setAxion(string axiom){
  current_string = axiom;
}

// Add the rule into the rule_left and rule_right array
// left : the input char of the rule
// right : the input string of the rule
void Lsystem::addRule(char left, string right){
  if(rule_num < 10){
    rule_left[rule_num] = left;
    rule_right[rule_num] = right;
    rule_num++;
  }
}

void Lsystem::generation(int iterations){
  for(int i = 0; i < iterations; i++){
    string temp_string = "";
    for(char c:current_string){
      bool find = false; // The boolean to tag if we find the rule or not
      for(int j = 0; j < rule_num; j++){
        if(rule_left[j] == c){ // if find the rule
          temp_string += rule_right[j];
          find = true;
          break;
        }
      }
      if(!find){ // if the rule doesn't find
        temp_string += c;
      }
    }
    current_string = temp_string;
  }
}


void Lsystem::draw(string filename){
  
  	// 1. Initialize the border 
    double min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    double temp_x = 0, temp_y = 0, temp_ang = 0; 

    // 2. Find the border of the img
    int temp_top = -1;
    state temp_stack[capacity]; 

    for (char c : current_string) {
        if (c == '[') {
            temp_top++;
            temp_stack[temp_top] = {temp_x, temp_y, temp_ang};
        }else if (c == ']') {
            temp_x = temp_stack[temp_top].x;
            temp_y = temp_stack[temp_top].y;
            temp_ang = temp_stack[temp_top].angle;
            temp_top--;
        }else if (c == 'F' || c == 'f') {
            double rad = temp_ang * PI / 180.0;
            temp_x += mov_length * cos(rad);
            temp_y += mov_length * sin(rad);
            
            // update the border
            if (temp_x < min_x) min_x = temp_x;
            if (temp_x > max_x) max_x = temp_x;
            if (temp_y < min_y) min_y = temp_y;
            if (temp_y > max_y) max_y = temp_y;
        }else if (c == '+') {
            temp_ang += mov_ang;
        }else if (c == '-') {
            temp_ang -= mov_ang;
        }
    }

    // 3. counting the parameter of viewBox (Add a 10% padding for white space.)
    double width = max_x - min_x;
    double height = max_y - min_y;
    double padding = 20.0; 
    
    ofstream outFile(filename);
    
    // use the counting parameter of border to do the viewBox
    outFile << "<svg xmlns='http://www.w3.org/2000/svg' width='800' height='800' "
            << "viewBox='" << min_x - padding << " " << -(max_y + padding) << " " 
            << width + 2 * padding << " " << height + 2 * padding << "'>" << endl;

    // file in the line
    // Reset the current location and angle to 0 and reset the top to -1
    curr_x = 0; curr_y = 0; curr_ang = 0; top = -1;
    for (char c : current_string) {
        if(c == '['){ // if c is "[", then push in state
	    	PushState();
	    }else if (c == 'F' || c == 'f') {
	      	double rad = curr_ang * PI / 180.0;
	      	double next_x = curr_x + mov_length * cos(rad);
	      	double next_y = curr_y + mov_length * sin(rad);
	
	      	if (c == 'F') {
	       	    cout << fixed << setprecision(2);
				cout << curr_x << "," << curr_y << "," << next_x << "," << next_y << endl;
	      		
				// generate a SVG line
	      		// In SVG, the Y-axis is positive downwards, which is opposite to the standard Cartesian coordinate system.
	      		outFile << "<line x1='" << curr_x << "' y1='" << -curr_y 
	      	        	<< "' x2='" << next_x << "' y2='" << -next_y 
	      	        	<< "' stroke='blue' stroke-width='1' />" << endl;
	     	}
	    
	      	curr_x = next_x;
	     	curr_y = next_y;
	     	
	    }else if(c == '+'){
	      curr_ang += mov_ang;
	    }else if(c == '-'){
	      curr_ang -= mov_ang;
	    }else if(c == ']'){
	      PopState();
	    }
    }

    outFile << "</svg>" << endl;
    outFile.close();
    // cout << "SVG file generated: << filename << endl;
}


int main()
{
  int case_num;
  cin >> case_num;
  for(int i = 0; i < case_num; i++){
    string axiom;
    int iterations;
    double step_length, angle;
    int n_rules;
    
    cin >> axiom >> iterations >> step_length >> angle >> n_rules;
    
    Lsystem lsystem(step_length, angle);
    lsystem.setAxion(axiom);
    for(int j = 0; j < n_rules; j++){
      char rule_left;
      string rule_right;
      cin >> rule_left >> rule_right;
      lsystem.addRule(rule_left, rule_right);
    }
    lsystem.generation(iterations);
    string filename = "Case_" + to_string(i + 1) + ".svg";
    lsystem.draw(filename);
  }
  return 0;
}
