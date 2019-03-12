/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Assignment 2 Part 1: Driving Route Finder
Names: Rutvik Patel, Kaden Dreger
ID: 1530012, 1528632
CCID: rutvik, kaden
CMPUT 275 Winter 2018

This file demonstrates an implementation of the famous dijkstra algorithm,
in which we light a "fire" at the root node and have the burn spread to all
notes of the graph, and adding to the tree based on the least "cost" to each
node/vertex.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/
#include "dijkstra.h"
#include "heap.h"

void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PLI>& tree) {
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
The dijkstra function takes the parameters:
    graph      : an instance of the weighted digraph (WDigraph)
    startVertex: the starting vertex, which will serve as the root of the tree
    tree       : an empty search tree, to construct the least cost path

It does not return anything.

The point of this function is to find the vertex in our point map based on the
passed in lat and lon values.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    tree = {};  // initialize tree to be empty
    BinaryHeap<pair<int, int> ,ll> events;  // empty events heap
    /* insert first element of graph into heap */
    pair<int, int> startPair(startVertex, startVertex);
    events.insert(startPair, 0);  // root should be itself

    while (events.size() > 0) {
        pair<pair<int, int>, ll> temp = events.min();  // getting root node
        events.popMin();  // pop root node
        pair<int, int> edge = temp.first;  // storing the edge
        ll weight = temp.second;  // storing the weight
        int u = edge.first;
        int v = edge.second;

        if (tree.find(v) == tree.end()) {
            tree[v] = make_pair(weight, u);
            for (auto iter = graph.neighbours(v);
                 iter != graph.endIterator(v); iter++) {
                pair<int, int> tempPair(v, *iter);  // making a temp pair
                ll cost = graph.getCost(v, *iter);  // getting cost of the edge
                events.insert(tempPair, weight + cost);  // inserting into the
                                                         // events heap
            }
        }
    }
}
