/***********************************************************************
* Author: suyou
* Date: Mar. 14, 2026
* Purpose: Assignment 1a -  Magic Square
***********************************************************************/
#include<iostream>
using namespace std;

/*
Magic Square Construction Rules :
1. Start by placing the first number in the middle column of the first row.
2. For the next number, move to the next column and the last row (as a wrap-around rule).
3. If the target cell is already occupied, place the number in the row directly below the previous number's position.
*/
class MagicSquare{
  
  // >> Overloading
  friend ostream& operator<<(ostream&, const MagicSquare&); // use friend to cout << set easily
  
  private: 
    int** magicArray; // a array that store magic array
    // the size of the magic square array
    // Both row number and column number are as same as size
    int size; 
    void generate();
  public:
    MagicSquare(int ArraySize); // Constructor
    ~MagicSquare(); // Destructor
};
// Constructor
MagicSquare::MagicSquare(int ArraySize):size(ArraySize){
  // Build a 2D Dynamic Arrays
  magicArray = new int*[size]; 
  for(int i = 0; i < size; i++){
    magicArray[i] = new int [size](); // set the default value of array as 0
  }
  this->generate();
}
// Destructor
MagicSquare::~MagicSquare(){
  // Delete the dynamic array and free the memory
  for(int i = 0; i < size; i++){
    delete[] magicArray[i];
  }
  delete [] magicArray;
}
// generate implementation
void MagicSquare::generate(){
  int curr_col = (0 + size - 1) / 2, curr_row = 0; // Declare two variable "curr_row" and "curr_col"
  int next_col = 0, next_row = 0; // Declare two variable "next_row" and "next_col"
  for (int num = 1; num <= size * size; num++){
			magicArray[curr_row][curr_col] = num; // key-in the number in the curr_row and curr_col
			
			  
			next_row = (size + (curr_row - 1)) % size; 
			next_col = (curr_col + 1) % size;
      // comfirm is there any number in the the place you want to key-in next time
      if (magicArray[next_row][next_col] != 0){ 
        // key-in the next number in the new row of curr_row and same column of curr_col
        next_row = (curr_row + 1) % size;
        next_col = curr_col;
      }
			
			curr_row = next_row;
			curr_col = next_col;
		}
}
// << Overloading
ostream& operator<<(ostream& os, const MagicSquare& A){
  for(int i = 0; i < A.size; i++){
    for(int j = 0; j < A.size; j++){
      os << A.magicArray[i][j];
      if(j != A.size - 1){
        os << ",";
      }
    }
    os << endl;
  }
  return os;
}

int main()
{
	int n, size; // n : the case of the input data, size : the size of the magic square
	cin >> n;
	for (int i = 0; i < n; i++){
		cin >> size;
		MagicSquare square(size);
		cout << square;
		
	}	
} 


