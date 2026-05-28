#include <iostream>
#include <algorithm> // for abs
using namespace std;

class DLinkedList;
class MinHeap;
class DLinkedNode{
	friend class DLinkedList;
    friend class MinHeap;
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
    private:
        DLinkedNode *first; // the pointer point to the first node of linked list
        DLinkedNode *last; // the pointer point to the last node of linked list
        int dlink_size; // count the length of double link list 
    public:
        DLinkedList(){ // Constructor
            first = nullptr;
            last = nullptr;
        }
        ~DLinkedList();  // Destructor
        void Insert_Back(double x_val, double y_val);
        double CountArea(DLinkedNode *apex);
};

class MinHeap{
    private:
        DLinkedList dlist; // Store the linked list we want to sort
        DLinkedNode** heap; // a pointer point to the array which store the pointer of the linked list node
        int capacity; // the maximum capacity of heap
        int heap_size; // the number of current nodes in the heap tree
    public:
        MinHeap(DLinkedList& list){ // Constructor
            dlist = list;
            capacity = dlist.dlink_size - 2; // the capacity of heap may exclude the first and last linked list node
            heap_size = capacity;
            heap = new DLinkedNode*[capacity + 1]; // we start the index from 1 to capacity
            initializeFromLinkList(); // when MinHeap construct, initialize the heap array from linked list
        }
        ~MinHeap(){
            delete [] heap;
        };
        void initializeFromLinkList(); // put the pointer of Linked list node in heap array from ptr to the last node
        void adjust(const int root); // adjust the heap tree
        void heapSort(int target_num); // heap sort
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
    if(dlist.dlink_size < 3){ // if link size is last then 3, it means there are only two node(first and last node)
        return;
    }

    DLinkedNode* curr = dlist.first->next;
    while(curr->next != dlist.last){
        heap_size += 1; 
        heap[heap_size] = curr; // put the linked node pointer in the heap array in heap_size index
        curr->heap_index = heap_size; // set the heap_index of linked list node as heap_size
        curr = curr->next;
    }
}

void MinHeap::adjust(const int root){
    // root: the first root we want to adjust
    DLinkedNode* temp = heap[root]; // store the linked list node pointer at the root of the heap
    int temp_index = root; // store the index of the root
    for(int i = 2* root; i <= heap_size; i *= 2){
        if(heap[i]->area > heap[i + 1]->area){ // find the child node with the minimum triangle area
            i++;
        } 
        if(temp->area < heap[i]->area){ 
            // if the area of child node is large than temp, then change with the parent node
            heap[i / 2] = heap[i];
            heap[i / 2]->heap_index = temp_index;
        }
        temp_index = i; // update the next heap root we want to sort
    }
    heap[temp_index / 2] = temp;
}

void MinHeap::heapSort(int target_num){
    // target_num: the output number of points
    // When the unsorted node(heap_size) is as same as target_num - 2, we stop sort.
    for(int i = heap_size / 2; i >= 1; i--){ // construct heap tree
        adjust(i);
    }
    for(int i = heap_size - 1; i >=  1; i --){ // sort the heap tree
        
        // Swap
        // swap the root of heap tree(heap[1]) and the last node of heap tree(heap[i + 1])
        DLinkedNode* removeNode = heap[i + 1];
        heap[i + 1] = heap[1];
        heap[1] = removeNode;
        // also, swap the heap_index of both node after we change heap array
        heap[i + 1]->heap_index = i + 1;
        heap[1]->heap_index = 1;


        // Update the linked list
        // The next pointer of prev node of heap[i + 1] point to the next node of heap[i + 1]
        DLinkedNode* temp = heap[i + 1]->prev; // update the temp to the prev node of heap[i + 1]
        temp->next = temp->next->next;
        temp->next->prev = temp;

        // Update the size of heap tree
        heap_size = i;

        // Update the area and adjust the heap tree
        if(temp != dlist.first){
            dlist.CountArea(temp);
            adjust(temp->heap_index);
        }
        if(temp->next != dlist.last || temp->next != nullptr){
            dlist.CountArea(temp->next);
            adjust(temp->next->heap_index);
        }

        // Detemine if heap_size is as same as target_num - 2
        if(heap_size == target_num - 2){ // if current heap_size is as same as target_num - 2, then break the loop
            break;
        }else{
            adjust(1);
        }
        
    }
}

int main(){
    cout << "test";
}