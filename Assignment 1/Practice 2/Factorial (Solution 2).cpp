/***********************************************************************
* Author: Ä¬¯§¼W(I133040010)
* Date: Mar. 14, 2026
* Purpose: Assignment 1b -  Factorial
***********************************************************************/
#include<iostream>
using namespace std;

const int MAX_INT = 500; // Set the maximum size of the array

class BigIntFac{
  
  friend ostream& operator<<(ostream&, const BigIntFac);
  
  private:
    int Fac_num[MAX_INT]; // Build an array to store all the number in the factorial
    int digitalCount; // Build an integer to count how many digital in this array
    
  public:
  	// Constructor
    BigIntFac(){
      digitalCount = 0; // when declare this class, we need to reset digital count to zero
      for(int i = 0; i < MAX_INT; i++){ // reset the digit in fac_num array
        Fac_num[i] = 0;
      }
    } // not need semicolon
    
    // Destructor
    ~BigIntFac(){ // when class variable meet the close brace(¥k¬A¸¹), then it will destruct it
    	// Beacause we use the static array , it will delete automatically
    	// Otherwise if we use dynamic array, we need to delete it manually
	  }
	  
	  // Operator Overloading
	  void operator*=(int n);// build a function to multiple all of the number in the array
    BigIntFac& operator=(const BigIntFac& other); // Assignment the value from this array to other array
    
    void calculate(int n); // build a function to calculate n!

}; // remember semicolon

void BigIntFac::operator*=(int n){
  int carry = 0;
  for(int i = 0; i < digitalCount; i++){ // count the product, and key-in all the digit into the fac_num array
    int temp;
    temp = Fac_num[i] * n + carry; 
    Fac_num[i] = temp % 10; 
    carry = temp / 10;
  }
  while(carry != 0){ // while carry out isn' t 0, then we need to expand the digitalcount, and put the number in the array;
    if(digitalCount < MAX_INT){
      Fac_num[digitalCount] = carry % 10;
      digitalCount++;
    }
    carry /= 10; // we put is outside to avoid the infinite loop happened
  }
}; // remember semicolon

// = Overloading
BigIntFac& BigIntFac::operator=(const BigIntFac& other) {
    if (this != &other) {
        this->digitalCount = other.digitalCount;
        for (int i = 0; i < other.digitalCount; i++) {
            this->Fac_num[i] = other.Fac_num[i];
        }
    }
    return *this;
}

// Calculate implementation
void BigIntFac::calculate(int n){
  digitalCount = 1; // we need to set the default digitalCount as 1
  Fac_num[0] = 1; // we need to set the default digit num of fac_num[0] as 1, or the product may be 0 in every case
  for(int i = 1; i <= n; i++){
    *this *= i;
  }
  if(n < 0){ // if n is < 0 ,then we should return the back to the code;
    return;
  }
}; // remember semicolon

ostream& operator<<(ostream& os, const BigIntFac A){
  for(int i = A.digitalCount - 1; i >= 0; i--){ // we need to output from the back to the front
      os << A.Fac_num[i];
    }
  os << endl;
  return os;
}

int main()
{
	int n;
	cin >> n;
	BigIntFac bif; // Because we declare class outside the for loop, so when be construct it, we need to reset the value
	for (int i = 0; i < n; i++){
	  int fac;
	  cin >> fac;
		bif.calculate(fac);
		cout << bif;
		
	}	
}// BigIntFac will destruct the array, when code execute to here


