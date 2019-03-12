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
#ifndef DIJKSTRA_h
#define DIJKSTRA_h

#include <unordered_set>
#include <unordered_map>
#include "wdigraph.h"
#include <stack>

typedef long long ll;
typedef pair<ll, int> PLI;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
The dijkstra function takes the parameters:
    graph      : an instance of the weighted digraph (WDigraph)
    startVertex: the starting vertex, which will serve as the root of the tree
    tree       : an empty search tree, to construct the least cost path

It does not return anything.

The point of this function is to find the vertex in our point map based on the
passed in lat and lon values.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PLI>& tree);



#endif /* DIJSKTRA_H */