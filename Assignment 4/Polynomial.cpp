/***********************************************************************
* Author: Ä¬¯§¼W(I133040010)
* Date: Apr. 19, 2026
* Purpose: Assignment 4 -  Polynomial
***********************************************************************/
#include <iostream>
using namespace std;

template <class T> class LinkedList; // forward declare

template <class T>
class LinkNode{
    friend class LinkedList<T>;

    template <class U>
    friend ostream& operator<<(ostream& os, const LinkedList<U>& Link); // overload operator <<
    
    private:
        T coeff;
        int expo;
        LinkNode<T>* link;
    public:
        LinkNode(const T& coefficient, const int exponent):coeff(coefficient),expo(exponent), link(nullptr){};
};

template <class T>
class LinkedList{
    private:
        LinkNode<T>* head; // the pointer point the head node of the linked list
        LinkNode<T>* last; // the pointer point the last node of the linked list  
    
    public:
        LinkedList(){ // Constructor
            head = nullptr;
            last = nullptr;
        };
        ~LinkedList(); // Destructor
        
        template <class U>
        friend ostream& operator<<(ostream& os, const LinkedList<U>& Link); // overload operator <<
        
        // insert next data in the back of the linked list
        void InsertNode(const T& insert_coeff, const int insert_expo); 

        LinkedList<T>* Addition(const LinkedList<T>& otherlink); // add the two linked list
        LinkedList<T>* Multiple(const LinkedList<T>& otherlink); // multiple the two linked list
        
};

template <class T>
LinkedList<T>::~LinkedList(){
    LinkNode<T>* delNode = this -> head;
    while(head != nullptr){
        delNode = head;
        head = head -> link;
        delete delNode;
    }
    last = nullptr;
}

template <class U>
ostream& operator<<(ostream& os, const LinkedList<U>& Link){
    LinkNode<U>* curr = Link.head;
    if(curr == nullptr){
        os << "0 0" << endl; // if the linked list is 0x^0
    }else{
        while(curr != nullptr){ // output "coeff expo"
            os << curr -> coeff << " " << curr -> expo;
            curr = curr -> link;
            if(curr != nullptr){
                os << " ";
            }
        }
        os << endl;
    }
    return os;
}


template <class T>
void LinkedList<T>::InsertNode(const T& insert_coeff, const int insert_expo){
    
    if (insert_coeff == 0){ // if coefficient of the insert node is 0, don't insert
        return;
    }

    // find the node which is same as insert_expo or first expo of the node which is smaller than insert_expo
    LinkNode<T> *curr_node = head;
    LinkNode<T> *prev_node = nullptr;
    while(curr_node != nullptr && curr_node -> expo > insert_expo){ 
        prev_node = curr_node;
        curr_node = curr_node -> link;
    }

    if(curr_node != nullptr && curr_node -> expo == insert_expo){ // if we find the same expo of the curr_node
        curr_node -> coeff += insert_coeff;
        if(curr_node -> coeff == 0){ // if the coeff of the node is 0, delete the node
            if(prev_node == nullptr){ // if curr_node is the first node
                head = curr_node -> link;
            }else{ // otherwise
                prev_node -> link = curr_node -> link;
            }
            if(curr_node == last){ // if the node is the last node of the linked list, we need to change the pointer "last"
                last = prev_node;
            }
            delete curr_node; // delete node in case memory leak
        }
    }else{
        LinkNode<T>* NextNode = new LinkNode<T>(insert_coeff, insert_expo); // create NextNode
        if(prev_node == nullptr){ // insert the node in the front of the linked list
            NextNode -> link = head;
            head = NextNode;
            if(last == nullptr){ // if the insert node is the first node of the linked list, we have to change the pointer "last"
                last = NextNode;
            }
        }else{ // insert node in the other location
            NextNode -> link = curr_node;
            prev_node -> link = NextNode;
            if(NextNode -> link == nullptr){ // if insert the node in the tail of the linked list
                last = NextNode;
            }
        }
    }
}

template <class T>
LinkedList<T>* LinkedList<T>::Addition(const LinkedList<T>& otherlink){
    LinkNode<T>* curr_node_A = this -> head;
    LinkNode<T>* curr_node_B = otherlink.head;
    LinkedList<T>* add = new LinkedList<T>;
    
    if(curr_node_A == nullptr){ // when this linkedlist is empty
        while(curr_node_B != nullptr){
            add -> InsertNode(curr_node_B -> coeff, curr_node_B -> expo);
            curr_node_B = curr_node_B -> link;
        }
        return add;
    }else if(curr_node_B == nullptr){ // when link linkedlist is empty
        while(curr_node_A != nullptr){
            add -> InsertNode(curr_node_A -> coeff, curr_node_A -> expo);
            curr_node_A = curr_node_A -> link;
        }
        return add;
    }

    while(curr_node_A != nullptr && curr_node_B != nullptr){
        if(curr_node_A -> expo == curr_node_B -> expo){ // if the expo of this linked list is same as the expo of otherlink linked list
            T new_coeff = curr_node_A -> coeff + curr_node_B -> coeff;
            int new_expo = curr_node_A -> expo;
            if(new_coeff != 0){ // if the new_coeff is not 0,we need to output 
                add -> InsertNode(new_coeff, new_expo);
            }
            curr_node_A = curr_node_A -> link;
            curr_node_B = curr_node_B -> link;
        }else if(curr_node_A -> expo > curr_node_B -> expo){ // if the expo of this linked list is larger than the expo of otherlink linked list
            add -> InsertNode(curr_node_A -> coeff, curr_node_A -> expo);
            curr_node_A = curr_node_A -> link;
        }else{ // if the expo of this linked list is smaller than the expo of otherlink linked list
            add -> InsertNode(curr_node_B -> coeff, curr_node_B -> expo);
            curr_node_B = curr_node_B -> link;
        }
    }
    if(curr_node_A == nullptr){
        while(curr_node_B != nullptr){
            add -> InsertNode(curr_node_B -> coeff, curr_node_B -> expo);
            curr_node_B = curr_node_B -> link;
        }
    }
    if(curr_node_B == nullptr){
        while(curr_node_A != nullptr){
            add -> InsertNode(curr_node_A -> coeff, curr_node_A -> expo);
            curr_node_A = curr_node_A -> link;
        }
    }
    return add;
}

template <class T>
LinkedList<T>* LinkedList<T>::Multiple(const LinkedList<T>& otherlink){
    LinkNode<T>* curr_node_A = this -> head;
    LinkNode<T>* curr_node_B = otherlink.head;
    LinkedList<T>* mul = new LinkedList<T>;

    if(curr_node_A == nullptr){ // when this linkedlist is empty
        while(curr_node_B != nullptr){
            mul -> InsertNode(curr_node_B -> coeff, curr_node_B -> expo);
            curr_node_B = curr_node_B -> link;
        }
        return mul;
    }else if(curr_node_B == nullptr){ // when link linkedlist is empty
        while(curr_node_A != nullptr){
            mul -> InsertNode(curr_node_A -> coeff, curr_node_A -> expo);
            curr_node_A = curr_node_A -> link;
        }
        return mul;
    }

    // Iterate through each term of the first polynomial this
    while(curr_node_A != nullptr){
        while(curr_node_B != nullptr){ // Iterate through each term of the second polynomial "otherlink"
            // a*x^c * b*x^d = a*b*x^(a+b)
            T new_coeff = (curr_node_A -> coeff) * (curr_node_B -> coeff); 
            int new_expo = (curr_node_A -> expo) + (curr_node_B -> expo);
            if(new_coeff != 0){
                mul -> InsertNode(new_coeff, new_expo);
            }
            curr_node_B = curr_node_B -> link;
        }
        curr_node_A = curr_node_A -> link;
        curr_node_B = otherlink.head;
    }
    return mul;
}
int main(){
    int case_num;
    cin >> case_num;
    for(int i = 0; i < case_num; i++){
        LinkedList<int>* link_A = new LinkedList<int>;
        LinkedList<int>* link_B = new LinkedList<int>;
        int term_A, term_B, coeff, expo;
        
        // InsertNode in polynomial B
        cin >> term_A;
        for(int i = 0; i < term_A; i++){
            cin >> coeff >> expo;
            link_A -> InsertNode(coeff, expo);
        }

        // InsertNode in polynomial B
        cin >> term_B;
        for(int i = 0; i < term_B; i++){
            cin >> coeff >> expo;
            link_B -> InsertNode(coeff, expo);
        }
        LinkedList<int>* addition;
        LinkedList<int>* multiple;
        addition = link_A -> Addition(*link_B);
        multiple = link_A -> Multiple(*link_B);
        cout << "Add: " << *addition;
        cout << "Mul: " << *multiple;
        
        // delete the pointer in case memory leak
        delete link_A;
        delete link_B;
        delete addition;
        delete multiple;
    }
}
