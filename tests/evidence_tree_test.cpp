
#include "../src/tree/EvidenceTree.h"

#include <iostream>
#include <cassert>
#include <sstream>

using namespace std;

int main() {

    EvidenceTree tree;


    Evidence e1 = {
        "EV003", "C001", "CMP001",
        "Payment Record", "Suspicious payment record",
        "Source1", "Financial", "Pending"
    };

    Evidence e2 = {
        "EV001", "C002", "CMP002",
        "UPI Record", "Repeated UPI identifier",
        "Source2", "Digital", "Verified"
    };

    Evidence e3 = {
        "EV002", "C001", "CMP003",
        "Complaint", "Multiple complaints received",
        "Source3", "Complaint", "Pending"
    };

    tree.insertEvidence(e1);
    tree.insertEvidence(e2);
    tree.insertEvidence(e3);

    cout << "Test 1 passed: Evidence insertion completed.\n";

    // Test 2: Display evidence
    stringstream buffer;
    streambuf* oldBuffer = cout.rdbuf(buffer.rdbuf());

    tree.displayEvidence();

    cout.rdbuf(oldBuffer);

    string output = buffer.str();

    assert(output.find("EV001") != string::npos);
    assert(output.find("EV002") != string::npos);
    assert(output.find("EV003") != string::npos);

    cout << "Test 2 passed: All evidence records displayed.\n";

    // Test 3: Search by campaign ID
    buffer.str("");
    buffer.clear();

    oldBuffer = cout.rdbuf(buffer.rdbuf());

    tree.searchByCampaignID("C001");

    cout.rdbuf(oldBuffer);

    output = buffer.str();

    assert(output.find("EV003") != string::npos);
    assert(output.find("EV002") != string::npos);
    assert(output.find("EV001") == string::npos);

    cout << "Test 3 passed: Campaign-based evidence search works.\n";

    // Test 4: Search for unavailable campaign
    buffer.str("");
    buffer.clear();

    oldBuffer = cout.rdbuf(buffer.rdbuf());

    tree.searchByCampaignID("C999");

    cout.rdbuf(oldBuffer);

    assert(buffer.str().find("No evidence found") != string::npos);

    cout << "Test 4 passed: Missing campaign handled correctly.\n";

    // Test 5: Clear tree
    tree.clear();

    buffer.str("");
    buffer.clear();

    oldBuffer = cout.rdbuf(buffer.rdbuf());

    tree.displayEvidence();

    cout.rdbuf(oldBuffer);

    assert(buffer.str().find("No evidence records available") != string::npos);

    cout << "Test 5 passed: Tree clearing works.\n";

    cout << "\nAll Evidence Tree tests passed successfully!\n";

    return 0;
}
