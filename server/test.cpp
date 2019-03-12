#include "heap.h"
#include <iostream>
using namespace std;

int main() {
    BinaryHeap<int, int> Bheap;

    cout << "size before insert: " << Bheap.size() << endl;
    Bheap.insert(9, 2);
    Bheap.insert(11, 5);
    Bheap.insert(54, 1);
    Bheap.insert(34, 8);
    cout << "size after insert: "<< Bheap.size() << endl;
    cout << Bheap.min().second << endl;

    return 0;
}