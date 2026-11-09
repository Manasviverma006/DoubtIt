
#include "graph.h"
#include <queue>
#include <vector>
#include <algorithm>
#include <functional>

Graph::Graph() {
    vertices = nullptr;
}

VertexNode* Graph::findVertex(string id) {
    VertexNode* temp = vertices;

    while (temp != nullptr) {
        if (temp->campaignId == id) {
            return temp;
        }

        temp = temp->next;
    }

    return nullptr;
}

void Graph::addVertex(string id) {
    if (findVertex(id) != nullptr) {
        return;
    }

    VertexNode* newVertex = new VertexNode;
    newVertex->campaignId = id;
    newVertex->head = nullptr;
    newVertex->next = vertices;

    vertices = newVertex;
}

void Graph::addRelationship(string source, string target,
                            string type, string identifier) {
    addVertex(source);
    addVertex(target);

    VertexNode* sourceVertex = findVertex(source);
    VertexNode* targetVertex = findVertex(target);

    EdgeNode* edge1 = new EdgeNode;
    edge1->destination = target;
    edge1->relationshipType = type;
    edge1->sharedIdentifier = identifier;
    edge1->next = sourceVertex->head;
    sourceVertex->head = edge1;

    EdgeNode* edge2 = new EdgeNode;
    edge2->destination = source;
    edge2->relationshipType = type;
    edge2->sharedIdentifier = identifier;
    edge2->next = targetVertex->head;
    targetVertex->head = edge2;
}

Graph::~Graph() {
    VertexNode* vertex = vertices;

    while (vertex != nullptr) {
        EdgeNode* edge = vertex->head;

        while (edge != nullptr) {
            EdgeNode* nextEdge = edge->next;
            delete edge;
            edge = nextEdge;
        }

        VertexNode* nextVertex = vertex->next;
        delete vertex;
        vertex = nextVertex;
    }
}
void Graph::displayGraph() {
    VertexNode* vertex = vertices;

    if (vertex == nullptr) {
        cout << "Graph is empty." << endl;
        return;
    }

    while (vertex != nullptr) {
        cout << "\n" << vertex->campaignId << " -> ";

        EdgeNode* edge = vertex->head;

        while (edge != nullptr) {
            cout << edge->destination
                 << " [" << edge->relationshipType
                 << ": " << edge->sharedIdentifier << "]";

            if (edge->next != nullptr) {
                cout << " -> ";
            }

            edge = edge->next;
        }

        cout << endl;
        vertex = vertex->next;
    }
}

void Graph::BFS(string start) {
    VertexNode* startVertex = findVertex(start);

    if (startVertex == nullptr) {
        cout << "Campaign not found in graph." << endl;
        return;
    }

    queue<string> q;
    vector<string> visited;

    q.push(start);
    visited.push_back(start);

    cout << "BFS Traversal: ";

    while (!q.empty()) {
        string current = q.front();
        q.pop();

        cout << current << " ";

        VertexNode* vertex = findVertex(current);
        EdgeNode* edge = vertex->head;

        while (edge != nullptr) {
            if (find(visited.begin(), visited.end(),
                     edge->destination) == visited.end()) {

                visited.push_back(edge->destination);
                q.push(edge->destination);
            }

            edge = edge->next;
        }
    }

    cout << endl;
}

void Graph::DFS(string start) {
    VertexNode* startVertex = findVertex(start);

    if (startVertex == nullptr) {
        cout << "Campaign not found in graph." << endl;
        return;
    }

    vector<string> visited;

    function<void(string)> dfsVisit = [&](string current) {
        visited.push_back(current);
        cout << current << " ";

        VertexNode* vertex = findVertex(current);
        EdgeNode* edge = vertex->head;

        while (edge != nullptr) {
            if (find(visited.begin(), visited.end(),
                     edge->destination) == visited.end()) {
                dfsVisit(edge->destination);
            }

            edge = edge->next;
        }
    };

    cout << "DFS Traversal: ";
    dfsVisit(start);
    cout << endl;
}





