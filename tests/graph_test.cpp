
#include <iostream>
#include "../src/graph/graph.h"

using namespace std;

int main() {
    Graph g;

    g.addRelationship("C001", "C005", "UPI", "shared01@upi.test");
    g.addRelationship("C001", "C003", "Bank test ID", "ACC-TEST-001");
    g.addRelationship("C001", "C002", "Phone test ID", "PH-TEST-001");
    g.addRelationship("C002", "C009", "Bank test ID", "ACC-TEST-002");

    cout << "\nGraph Display:\n";
    g.displayGraph();

    cout << "\n";
    g.BFS("C001");

    cout << "\n";
    g.DFS("C001");

    return 0;
}
