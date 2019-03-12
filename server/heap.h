/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Assignment 2 Part 1: Driving Route Finder
Names: Rutvik Patel, Kaden Dreger
ID: 1530012, 1528632
CCID: rutvik, kaden
CMPUT 275 Winter 2018

This file is an implementation of the binary heap class, with the following
methods: min, insert, popMin, and size.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

#ifndef _HEAP_H_
#define _HEAP_H_

#include <vector>
#include <utility> // for pair
#include <math.h>
#include <algorithm>
#include <iostream>
using namespace std;
// T is the type of the item to be held
// K is the type of the key associated with each item in the heap
// The only requirement is that K is totally ordered and comparable via <
template <class T, class K>
class BinaryHeap {
 public:
    // return the minimum element in the heap
    pair<T, K> min() const {
        // return the root node
        return heap[0];
    }

    // insert an item with the given key
    // if the item is already in the heap, will still insert a new copy with this key
    void insert(const T& item, const K& key) {
        // let v be a new vertex at the end of the array/tree
        pair<T, K> v(item, key);
        heap.push_back(v);
        int index = heap.size() - 1;  // end of the tree
        sift_up(index);  // sift upwards to maintain heap property
        //cout << heap.size() << " " << heap[0].second << endl;
    }


    // pop the minimum item from the heap
    void popMin() {
        // load the root node
        /*swapping root and end node*/
        iter_swap(heap.begin(), heap.begin() + heap.size() - 1);
        heap.pop_back();  // popping off last item
        sift_down(0);  // sift downwards to maintain heap property
        //cout << heap.size() << " " << heap[0].second << endl;
    }

    // returns the number of items held in the heap
    int size() const {
        // return the number of items in the heap
        return heap.size();
    } 

    private:
    // the array holding the heap
    vector<pair<T, K> > heap;

    // the following two functions are modified implementations from the site:
    // https://www.techiedelight.com/min-heap-max-heap-implementation-c/

    // sifting upwards recursively to maintain heap property
    void sift_up(int i) {
        if (i && heap[goParent(i)].second > heap[i].second) {
            swap(heap[i], heap[goParent(i)]);
            sift_up(goParent(i));
        }
    }

    // sifting downwards recursively to maintain heap property
    void sift_down(int i) {
        unsigned int leftIndex = goLeft(i);
        unsigned int rightIndex = goRight(i);
        int smallVal = i;

        if (leftIndex < heap.size() && heap[leftIndex].second < heap[i].second) {
            smallVal = leftIndex;
        }
        if (rightIndex < heap.size() && heap[rightIndex].second < heap[smallVal].second) {
            smallVal = rightIndex;
        }

        if (smallVal != i) {
            swap(heap[i], heap[smallVal]);
            sift_down(smallVal);
        }
    }

    // returning the index of the left child
    int goLeft(int index) {
        return (2*index + 1);
    }

    // returning the index of the right child
    int goRight(int index) {
        return(2*index + 2);
    }

    // returning the index of the parent
    int goParent(int index) {
        return (floor((index - 1) / 2));
    }

};

#endif  /*HEAP_H*/
