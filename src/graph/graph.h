
#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
using namespace std;

struct EdgeNode {
    string destination;
    string relationshipType;
    string sharedIdentifier;
    EdgeNode* next;
};

struct VertexNode {
    string campaignId;
    EdgeNode* head;
    VertexNode* next;
};

class Graph {
private:
    VertexNode* vertices;

    VertexNode* findVertex(string id);
    void addVertex(string id);

public:
    Graph();
    ~Graph();

    void addRelationship(string source, string target,
                         string type, string identifier);

    void displayGraph();
    void BFS(string start);
    void DFS(string start);
};

#endif
