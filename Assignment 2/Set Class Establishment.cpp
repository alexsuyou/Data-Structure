/***********************************************************************
* Author: 蘇祐增(I133040010)
* Date: Mar. 30, 2026
* Purpose: Assignment 2 - Establishment of the Set Class
***********************************************************************/
#include <iostream>
#include <algorithm> // for min
using namespace std;


// Defination the data type of the set
template <class T> // T represents the data type of the elements stored in the set

class TSet{
  // << Overloading
  template <class U>
  friend ostream& operator<<(ostream&, const TSet<U>&); // use friend to cout << set easily, and s mean the set you want to output
  
  private:
    
    T *elementsArray; // Create a elementsArray with T data type
    int size; // Size is the current size of the elementsArray
    int capacity; // Capacity is the size of the set
    bool isFull() const; // Determine if the size is same as capacity
    int findIndex(const T& element) const;
  public:
    
    // Constructor & Destructor
    TSet(int SetCapacity);
    ~TSet();
    
    // add implementation
    void add(const T& element); // add element into set
    
    // Operator Overloading
    TSet<T> operator+(const TSet<T>& otherSet); // Union (A + B)
    TSet<T> operator*(const TSet<T>& otherSet); // Intersection (A * B)
    TSet<T> operator-(const TSet<T>& otherSet); // Difference (A - B)
    bool operator>=(const TSet<T>& otherSet); // Containment (A >= B)
    bool operator%(const T& x) const; // find if x is in the set or not and return the boolean
    
    
    
    
     
};
// Constructor
template <class T>
TSet<T>::TSet(int SetCapacity):capacity(SetCapacity) // Default the capacity of set as SetCapacity
{
  size = 0; // default the size of the set as 0
  elementsArray = new T[capacity]; // set elementsArray as capacity size
}
// Destructor
template <class T>
TSet<T>::~TSet(){
  if(elementsArray != nullptr){
    delete[] elementsArray; // Free the dynamic dynamically allocated array space
    elementsArray = nullptr; // Set elementsArray as nullptr in case of becoming dangling pointer（懸空指標）
  }
}
// isFull implementation
// inline means to suggests the compiler to replace function calls with the actual code.
template <class T>
inline bool TSet<T>::isFull() const 
{
  if(size == capacity){
    return true;
  }else{
    return false;
  }
}
// findInx implementation
template <class T>
int TSet<T>::findIndex(const T& element) const
{
  for(int i = 0; i < size; i++){ // Find the index of the element
    if(elementsArray[i] == element){
      return i;
    }
  }
  return -1;
}
// Add element implementation
template <class T>
void TSet<T>::add(const T& element){
  if(!operator%(element)){ // Determine if the element is in the set
    if(!isFull()){ // Detemine if the set is not full
      int index = size; // Default index is size in case all of the elements in the set issmaller than element
      for(int i = 0; i < size; i++){ // Find the index to insert element
        if(elementsArray[i] > element){
          index = i;
          break;
        }
      }
      for(int i = size; i > index; i--){ // Rearangement the elements from index + 1 to size
        elementsArray[i] = elementsArray[i - 1];
      }
      elementsArray[index] = element;
      size += 1; // Update the size of the set
    }else{
      throw "The capacity is out of range.";
    }
  }
}
// Union implementation
template <class T>
TSet<T> TSet<T>::operator+(const TSet<T>& otherSet){
  TSet<T> ans(this->size + otherSet.size); // set the capacity of ans as this.capcity + otherSet.capcity
  for(int i = 0; i < this->size; i++){ // use add implementation to add the elements from the elementsArray of this set
    ans.add(this->elementsArray[i]);
  }
  for(int i = 0; i < otherSet.size; i++){ // use add implementation to add the elements from the elementsArray of otherSet
    ans.add(otherSet.elementsArray[i]);
  }
  return ans;
}
// Intersection implementation
template <class T>
TSet<T> TSet<T>::operator*(const TSet<T>& otherSet){
  TSet<T> ans(min(this->size, otherSet.size)); // Sets the capacity of the ans set to the minimum of both sets.
  for(int i = 0; i < otherSet.size; i++){ // Find if the elements in this set are also in otherSet
    if(operator%(otherSet.elementsArray[i])){
      ans.add(otherSet.elementsArray[i]);
    }
  }
  return ans;
}
// Difference implementation
template <class T>
TSet<T> TSet<T>::operator-(const TSet<T>& otherSet){
  TSet<T> ans(this->size); // Set the capcity of the ans set to this set
  for(int i = 0; i < this->size; i++){ // Find if the elements are in this set but not in otherSet
    if(!otherSet.operator%(this->elementsArray[i])){
      ans.add(this->elementsArray[i]);
    }
  }
  return ans;
}
// Containment implementation
template <class T>
bool TSet<T>::operator>=(const TSet<T>& otherSet){ // Find if otherSet is the containment of this set
  for(int i = 0; i < otherSet.size; i++){ // Find if all of the elements from otherSet are also in this set
    if(!operator%(otherSet.elementsArray[i])){
      return false;
    }
  }
  return true;
}
// Belonging implementation
template <class T>
bool TSet<T>::operator%(const T& x) const
{
  if(findIndex(x) != -1){
    return true;
  }else{
    return false;
  }
}
// << implementation
template <class U>
ostream& operator<<(ostream& os, const TSet<U>& s){
  for(int i = 0; i < s.size; i++){
    os << s.elementsArray[i];
    if(i != s.size - 1){ // if i is not the last index, then output comma
      os << ",";
    }
  }
  return os;
}

int main()
{
  int case_num;
  cin >> case_num;
  for(int i = 0; i < case_num; i++){
    
    // Capacity_A, Capacity_B : get the Capacity of set A and set B
    // x : get the number which we want to know if it is in the set    
    unsigned int Capacity_A, Capacity_B; // the maximun of the capcity of the set may as many as 2^32 - 1
	unsigned int x; //  also the val of the number may be as large as 2^32 - 1

    char comma; // get the comma between each number
    unsigned int val; // get the number which we want to store in the set
    
    cin >> Capacity_A; 
    
    // Declare set A with Capacity_A
    // use unsigned int can store the int from 0 to 2^32 -1
    TSet<unsigned int> A(Capacity_A); 
    for(unsigned int i = 0; i < Capacity_A; i++){
      cin >> comma >> val;
      A.add(val);
    }
    
    cin >> Capacity_B; 
    
    // Declare set B with Capacity_B
    // use unsigned int can store the int from 0 to 2^32 -1
    TSet<unsigned int> B(Capacity_B); 
    for(unsigned int i = 0; i < Capacity_B; i++){
      cin >> comma >> val;
      B.add(val);
    }
    
    cin >> x;
    
    // Output
    cout << "Test Case #" << i + 1 << ":" << endl;
    cout << "A:" << A << endl;
    cout << "B:" << B << endl;
    cout << "A+B:" << A + B << endl;
    cout << "A*B:" << A * B << endl;
    cout << "A-B:" << A - B << endl;
    cout << "B-A:" << B - A << endl;
    if(A >= B){
      cout << "A contain B" << endl;
    }else{
      cout << "A does not contain B" << endl;
    }
    if(B >= A){
      cout << "B contain A" << endl;
    }else{
      cout << "B does not contain A" << endl;
    }
    if(A % x){
      cout << x << " is in A" << endl;
    }else{
      cout << x << " is not in A" << endl;
    }
    if(B % x){
      cout << x << " is in B" << endl;
    }else{
      cout << x << " is not in B" << endl;
    }
    if(i != case_num - 1){
      cout << endl;
    }
  }
  return 0;
}
