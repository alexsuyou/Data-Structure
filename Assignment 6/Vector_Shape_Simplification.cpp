/***********************************************************************
* Author: 蘇祐增 (I133040010)
* Date: May 29, 2026
* Purpose: Assignment 6 - Vector Shape Simplification
***********************************************************************/
#include <iostream>
#include <algorithm> // for abs
#include <iomanip> // for fixed and setprecision
using namespace std;

class DLinkedList;
class MinHeap;
class DLinkedNode{
	friend class DLinkedList;
    friend class MinHeap;
    friend ostream& operator<<(ostream& os, DLinkedList* link); // overload operator <<
    private:
        double x, y; // x and y cooridinate
        double area; // the area consist from this node, previous node and next node
        DLinkedNode *prev; // pointer point to the previous node
        DLinkedNode *next; // pointer point to the next node
        int heap_index; // the index of the node in the minimum heap tree
    public:
        DLinkedNode(double x_val = 0, double y_val = 0){ // Constructor
            x = x_val;
            y = y_val;
            area = 0;
            prev = nullptr;
            next = nullptr;
            heap_index = 0;
        }
        ~DLinkedNode(){}; // Destructor

};
class DLinkedList{
    friend class MinHeap;
    friend ostream& operator<<(ostream& os, DLinkedList* link); // overload operator <<
    private:
        DLinkedNode *first; // the pointer point to the first node of linked list
        DLinkedNode *last; // the pointer point to the last node of linked list
        int dlink_size; // count the length of double link list 
    public:
        DLinkedList(){ // Constructor
            first = nullptr;
            last = nullptr;
            dlink_size = 0;
        }
        ~DLinkedList();  // Destructor
        void Insert_Back(double x_val, double y_val); // insert node in the back of the linked list
        double CountArea(DLinkedNode *apex); // count the point of the area
};

class MinHeap{
    private:
        DLinkedList* dlist; // Store the linked list we want to sort
        DLinkedNode** heap; // a pointer point to the array which store the pointer of the linked list node
        int capacity; // the maximum capacity of heap
        int heap_size; // the number of current nodes in the heap tree
    public:
        MinHeap(DLinkedList* list){ // Constructor
            dlist = list;

            if (dlist == nullptr || dlist->dlink_size < 3) {
                capacity = 0;
                heap_size = 0;
                heap = nullptr; 
                return;
            }

            capacity = dlist->dlink_size - 2; // the capacity of heap may exclude the first and last linked list node
            heap_size = capacity;
            heap = new DLinkedNode*[capacity + 1]; // we start the index from 1 to capacity
            initializeFromLinkList(); // when MinHeap construct, initialize the heap array from linked list
        }
        ~MinHeap(){
            delete [] heap;
        };

        // calculates the initial triangle area for each internal node 
        // and populates the heap array with their pointers
        void initializeFromLinkList(); 

        void adjust_TopToButtom(const int root); // adjust the heap tree from top to buttom
        void adjust_ButtomToTop(const int root); // adjust the heap tree from buttom to top
        
        // when recalculate the triangle area, update the heap tree
        // using adjust_TopToButtom and adjust_ButtomToTop
        void update(const int index);

        void simplify(int target_num); // use heap sort to simplify the node
};

DLinkedList::~DLinkedList(){  // Destructor
    DLinkedNode* delNode = this->first;
    while(first != nullptr){
        delNode = first;
        first = first->next;
        delete delNode;
    }
    last = nullptr;
}

ostream& operator<<(ostream& os, DLinkedList* link){
    DLinkedNode* curr = link->first;
    while(curr != nullptr){
        os << fixed << setprecision(2) << curr->x << "," <<curr->y << endl;
        curr = curr->next;
    }
    return os;
}

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
    dlink_size++; // if insert a node, the length of double link list + 1
}

double DLinkedList::CountArea(DLinkedNode *apex){
    // Find the x and y of the last node, current node and next node
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

void MinHeap::initializeFromLinkList(){
    if(dlist->dlink_size < 3){ // if link size is last then 3, it means there are only two node(first and last node)
        return;
    }

    DLinkedNode* curr = dlist->first->next;
    int curr_heap_index = 0;
    while(curr != dlist->last){
        curr->area = dlist->CountArea(curr); // count the triangle area
        curr_heap_index += 1; 
        heap[curr_heap_index] = curr; // put the linked node pointer in the heap array in heap_size index
        curr->heap_index = curr_heap_index; // set the heap_index of linked list node as heap_size
        curr = curr->next;
    }
}

void MinHeap::adjust_TopToButtom(const int root){
    // root: the first root we want to adjust from top to buttom
    DLinkedNode* temp = heap[root]; // store the linked list node pointer at the root of the heap
    int temp_index = root; // store the index of the root
    for(int i = 2* root; i <= heap_size; i *= 2){
        if(i < heap_size && heap[i]->area > heap[i + 1]->area){ // find the child node with the minimum triangle area
            i++;
        } 
        if(temp->area <= heap[i]->area){ 
            // if the area of temp node is small than child, then break
            break;
        }
        heap[i / 2] = heap[i];
        heap[i / 2]->heap_index = temp_index;
        temp_index = i; // update the next heap root we want to sort
    }
    heap[temp_index] = temp;
    heap[temp_index]->heap_index = temp_index;
}

void MinHeap::adjust_ButtomToTop(const int root){
    // root: the first root we want to adjust form buttom to top
    DLinkedNode* temp = heap[root]; // store the linked list node pointer at the root of the heap
    int temp_index = root; // store the index of the root
    while(temp_index != 1 && heap[temp_index / 2]->area > temp->area){
        heap[temp_index] = heap[temp_index / 2];
        heap[temp_index]->heap_index = temp_index;
        temp_index /= 2; // update the next heap root we want to sort
    }
    heap[temp_index] = temp;
    heap[temp_index]->heap_index = temp_index;
}

void MinHeap::update(const int index){ 
    // when recalculate the triangle area, update the heap tree
    // using adjust_TopToButtom and adjust_ButtomToTop
    if(index >= 1 && index <= heap_size){
        adjust_TopToButtom(index);
        adjust_ButtomToTop(index);
    }
}

void MinHeap::simplify(int target_num){
    // target_num: the output number of points
    // When the unsorted node(heap_size) is as same as target_num - 2, we stop sort.
    for(int i = heap_size / 2; i >= 1; i--){ // construct heap tree
        adjust_TopToButtom(i);
    }

    // Detemine if heap_size is as same as target_num - 2
    while(heap_size > target_num - 2){ // sort the heap tree
        
        // move the last node of heap tree(heap[heap_size]) to the root of heap tree(heap[1]) 
        DLinkedNode* removeNode = heap[1];
        heap[1] = heap[heap_size];
        heap[1]->heap_index = 1;

        // update the size of heap tree
        heap_size -= 1;

        adjust_TopToButtom(1);

        // Update the linked list
        DLinkedNode* tempPrev = removeNode->prev; // the pointer point to the prev node of the remove node
        DLinkedNode* tempNext = removeNode->next; // the pointer point to the next node of the remove node
        tempPrev->next = tempNext;
        tempNext->prev = tempPrev;

        // delete the remove node
        delete removeNode;



        // Update the area and adjust the heap tree
        if(tempPrev != dlist->first){
            tempPrev->area = dlist->CountArea(tempPrev);
            update(tempPrev->heap_index);
            
        }
        if(tempNext != dlist->last){
            tempNext->area = dlist->CountArea(tempNext);
            update(tempNext->heap_index);
        }
        
    }
}

int main(){
    int test_case;
    cin >> test_case;
    for(int i = 0; i < test_case; i++){
        int init_point_num, target_point_num;
        cin >> target_point_num >> init_point_num;
        DLinkedList* vector_list = new DLinkedList();
        for(int j = 0; j < init_point_num; j++){
            double x, y; // input the x cooridinate and y cooridinate of esch point
            cin >> x >> y;
            vector_list->Insert_Back(x, y);
        }
        MinHeap minheap(vector_list);
        minheap.simplify(target_point_num);
        cout << vector_list;
        cout << endl;

        delete vector_list; // in case memory leakage
    }
}