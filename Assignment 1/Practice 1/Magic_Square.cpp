/***********************************************************************
* Author: Ä¬¯§¼W(I133040010)
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
void key(int, int, int, int**); // Build a function "key" to key in all the number in the magic square 

int main()
{
	int n, size; // n : the case of the input data, size : the size of the magic square
	cin >> n;
	for (int i = 0; i < n; i++){
		cin >> size;
		
		int **square = new int*[size](); // Declare a 2 dimension array called square, and add () means set the value of the array as 0
		for (int i = 0; i < size; i++){ // consule in all of the data into the array 
		  square[i] = new int[size]();
		}
		
		int row = 0, col = (0 + size - 1) / 2;
		key(size, row, col, square);
		
		for (int j = 0; j < size; j++){ // Consule out all of number in the array
			for (int k = 0; k < size; k++){
				cout << square[j][k];
				if (k < size - 1){
					cout << ", ";
				}
			}
			cout << endl;
		}
		// delete the poineter
		for (int i = 0; i < size; i++){ 
		  delete[] square[i];
		}
		delete[] square;
	}	
} 

void key(int size ,int row ,int col, int **square){
  for (int num = 1; num <= size * size; num++){
			if (square[row][col] != 0){ // comfirm is there any number in the the place you want to key-in next time
			  // if the condition is correct, the key-in in the next row of the original place
			  row = (row + 1) % size + 1; 
			  col = (size + (col - 1)) % size;
			}
			square[row][col] = num;
			//key-in the next number in the last row and next column
			row = (size + (row - 1)) % size;
			col = (col + 1) % size;
			
		}
}
