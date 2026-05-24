#include<iostream>
#include<algorithm> // for abs

class DLinkedList;
class DLinkedNode{
	friend class DLinkedList;
    private:
        double x, y; // x and y cooridinate
        double area; // the area consist from this node, previous node and next node
        DLinkedNode *prev; // pointer point to the previous node
        DLinkedNode *next; // pointer point to the next node
    public:
        DLinkedNode(double x_val = 0, double y_val = 0){ // Constructor
            x = x_val;
            y = y_val;
            area = 0;
            prev = nullptr;
            next = nullptr;
        }
        ~DLinkedNode(); // Destructor
        

};
class DLinkedList{
    private:
        DLinkedNode *first; // the pointer point to the first node of linked list
        DLinkedNode *last; // the pointer point to the last node of linked list
    public:
        DLinkedList(); // Constructor
        ~DLinkedList(); // Destructor
        void Insert_Back(double x_val, double y_val);
        double CountArea(DLinkedNode *apex);
};

void DLinkedList::Insert_Back(double x_val, double y_val){
    if(first){ // if the double linked list is not empty
        last->next = new DLinkedNode(x_val, y_val);
        last->next->prev = last;
        last = last->next;
    }else{ // if the Double Linked list is empty
        first = last = new DLinkedNode(x_val, y_val);
    }
    if(first != last && first->next != last){ 
        // first = last means there is only a node
        // first->next = last means there is only two node 
        last->prev->area = CountArea(last->prev);
    }
}

double DLinkedList::CountArea(DLinkedNode *apex){
    double last_x = apex->prev->x;
    double last_y = apex->prev->y;
    double curr_x = apex->x;
    double curr_y = apex->y;
    double next_x = apex->next->x;
    double next_y = apex->next->y;
    double area;
    area = 0.5 * abs(last_x * (curr_y - next_y) + curr_x * (next_y - last_y) + next_x * (last_y - curr_y));
    return area;
}


int main(){}