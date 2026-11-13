
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "risk/RiskAssessment.h"
#include "risk/MaxHeap.h"

#include "common/Campaign.h"
#include "common/Complaint.h"

#include "linked_list/CampaignList.h"
#include "linked_list/ComplaintList.h"

#include "hash_table/HashTable.h"
#include "graph/graph.h"
#include "tree/EvidenceTree.h"

using namespace std;


// =================================
// CSV PARSING FUNCTION
// =================================

vector<string> parseCSVLine(string line) {

    vector<string> fields;
    string field;
    bool insideQuotes = false;
for (size_t i = 0; i < line.length(); i++){

        char ch = line[i];

        if (ch == '"') {

            if  (insideQuotes && i + 1 < line.length()
                && line[i + 1] == '"') {

                field += '"';
                i++;
            }
            else {
                insideQuotes = !insideQuotes;
            }
        }

        else if (ch == ',' && !insideQuotes) {

            fields.push_back(field);
            field.clear();
        }

        else {
            field += ch;
        }
    }

    fields.push_back(field);

    return fields;
}


// =================================
// MAIN FUNCTION
// =================================

int main() {

    CampaignList list;
    HashTable hashTable;
    ComplaintList complaintList;
    Graph graph;
    EvidenceTree evidenceTree;


    string line;


    // =================================
    // LOAD CAMPAIGN DATA
    // =================================

    ifstream file("data/campaigns.csv");

    if (!file.is_open()) {

        cout << "Unable to open campaigns.csv" << endl;
        return 1;
    }

    getline(file, line);

    int loaded = 0;
    int skipped = 0;

    while (getline(file, line)) {

        if (line.empty()) continue;

        vector<string> fields = parseCSVLine(line);

        if (fields.size() != 9) {

            skipped++;
            continue;
        }

        Campaign c;

        c.campaign_id = fields[0];
        c.campaign_name = fields[1];
        c.organization_name = fields[2];
        c.website = fields[3];
        c.upi_id = fields[4];
        c.bank_account_test_id = fields[5];
        c.phone_test_id = fields[6];
        c.record_type = fields[7];
        c.review_status = fields[8];

        list.insertCampaign(c);
        hashTable.insertCampaign(c);

        loaded++;
    }

    file.close();


    // =================================
    // LOAD COMPLAINT DATA
    // =================================

    ifstream complaintFile("data/complaints.csv");

    if (!complaintFile.is_open()) {

        cout << "Unable to open complaints.csv" << endl;
        return 1;
    }

    getline(complaintFile, line);

    int complaintsLoaded = 0;
    int complaintsSkipped = 0;

    while (getline(complaintFile, line)) {

        if (line.empty()) continue;

        vector<string> fields = parseCSVLine(line);

        if (fields.size() != 10) {

            complaintsSkipped++;
            continue;
        }

        Complaint c;

        c.complaint_id = fields[0];
        c.campaign_id = fields[1];
        c.date_received = fields[2];
        c.report_channel = fields[3];
        c.complaint_description = fields[4];
        c.complaint_category = fields[5];
        c.complaint_status = fields[6];
        c.record_origin = fields[7];
        c.record_type = fields[8];
        c.related_identifier = fields[9];

        complaintList.insertComplaint(c);

        complaintsLoaded++;
    }

    complaintFile.close();


    // =================================
    // LOAD EVIDENCE DATA INTO TREE
    // =================================

    ifstream evidenceFile("data/evidence.csv");

    if (!evidenceFile.is_open()) {

        cout << "Unable to open evidence.csv" << endl;
        return 1;
    }

    getline(evidenceFile, line);

    int evidenceLoaded = 0;
    int evidenceSkipped = 0;

    while (getline(evidenceFile, line)) {

        if (line.empty()) continue;

        vector<string> fields = parseCSVLine(line);

        if (fields.size() != 8) {

            evidenceSkipped++;
            continue;
        }

        Evidence e;

        e.evidenceID = fields[0];
        e.campaignID = fields[1];
        e.complaintID = fields[2];
        e.evidenceType = fields[3];
        e.description = fields[4];
        e.sourceReference = fields[5];
        e.recordType = fields[6];
        e.verificationStatus = fields[7];

        evidenceTree.insertEvidence(e);

        evidenceLoaded++;
    }

    evidenceFile.close();


    // =================================
    // LOAD RELATIONSHIP DATA INTO GRAPH
    // =================================

    ifstream relationshipFile("data/relationships.csv");

    if (!relationshipFile.is_open()) {

        cout << "Unable to open relationships.csv" << endl;
        return 1;
    }

    getline(relationshipFile, line);

    int relationshipsLoaded = 0;
    int relationshipsSkipped = 0;

    while (getline(relationshipFile, line)) {

        if (line.empty()) continue;

        vector<string> fields = parseCSVLine(line);

        if (fields.size() != 7) {

            relationshipsSkipped++;
            continue;
        }

        string source = fields[1];
        string target = fields[2];
        string relationshipType = fields[3];
        string sharedIdentifier = fields[4];

        if (source.empty() || target.empty()) {

            relationshipsSkipped++;
            continue;
        }

        graph.addRelationship(
            source,
            target,
            relationshipType,
            sharedIdentifier
        );

        relationshipsLoaded++;
    }

    relationshipFile.close();


    // =================================
    // DATA SUMMARY
    // =================================

    cout << "\n========== DOUBTIT ==========" << endl;

    cout << "\nCAMPAIGN DATA SUMMARY" << endl;

    cout << "Campaigns loaded: "
         << loaded << endl;

    cout << "Invalid campaign rows skipped: "
         << skipped << endl;


    cout << "\nCOMPLAINT DATA SUMMARY" << endl;

    cout << "Complaints loaded: "
         << complaintsLoaded << endl;

    cout << "Invalid complaint rows skipped: "
         << complaintsSkipped << endl;


    cout << "\nEVIDENCE DATA SUMMARY" << endl;

    cout << "Evidence records loaded: "
         << evidenceLoaded << endl;

    cout << "Invalid evidence rows skipped: "
         << evidenceSkipped << endl;


    cout << "\nGRAPH DATA SUMMARY" << endl;

    cout << "Relationships loaded: "
         << relationshipsLoaded << endl;

    cout << "Invalid relationship rows skipped: "
         << relationshipsSkipped << endl;


    // =================================
    // MAIN MENU
    // =================================

    int choice;
    string identifier;
    string campaignId;

    // =================================
// RISK SCORING AND MAX HEAP
// =================================

vector<RiskResult> riskResults =
    calculateRiskScores(list, complaintList);

MaxHeap riskHeap;

for (const RiskResult& result : riskResults) {
    riskHeap.insert(result);
}

cout << "\nRisk scores calculated for "
     << riskResults.size()
     << " campaigns." << endl;

    do {

        cout << "\n========== DOUBTIT MENU ==========" << endl;

        cout << "\n--- IDENTIFIER VERIFICATION ---" << endl;

        cout << "1. Search UPI ID" << endl;
        cout << "2. Search Bank Account" << endl;
        cout << "3. Search Phone Number" << endl;
        cout << "4. Display Shared Identifiers" << endl;


        cout << "\n--- COMPLAINT MANAGEMENT ---" << endl;

        cout << "5. Search Complaints by Campaign ID" << endl;


        cout << "\n--- GRAPH OPERATIONS ---" << endl;

        cout << "6. Display Relationship Graph" << endl;
        cout << "7. BFS Traversal" << endl;
        cout << "8. DFS Traversal" << endl;


        cout << "\n--- EVIDENCE MANAGEMENT ---" << endl;

        cout << "9. Display All Evidence" << endl;
        cout << "10. Search Evidence by Campaign ID" << endl;

        cout << "11. Display Campaign Risk Ranking" << endl;
        cout << "12. Exit" << endl;
        


        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {

            cin.clear();
            cin.ignore(10000, '\n');

            cout << "Invalid input. Enter a number." << endl;
            continue;
        }


        switch (choice) {

            // =================================
            // IDENTIFIER VERIFICATION
            // =================================

            case 1:

                cout << "Enter UPI ID: ";
                cin >> identifier;

                hashTable.searchUPI(identifier);

                break;


            case 2:

                cout << "Enter Bank Account ID: ";
                cin >> identifier;

                hashTable.searchBankAccount(identifier);

                break;


            case 3:

                cout << "Enter Phone Number ID: ";
                cin >> identifier;

                hashTable.searchPhoneNumber(identifier);

                break;


            case 4:

                hashTable.displaySharedUPIs();

                hashTable.displaySharedBankAccounts();

                hashTable.displaySharedPhoneNumbers();

                break;


            // =================================
            // COMPLAINT MANAGEMENT
            // =================================

            case 5:

                cout << "Enter Campaign ID: ";
                cin >> campaignId;

                cout << "\nTotal complaints: "
                     << complaintList.countComplaintsForCampaign(campaignId)
                     << endl;

                complaintList.displayComplaintsForCampaign(campaignId);

                break;


            // =================================
            // GRAPH OPERATIONS
            // =================================

            case 6:

                graph.displayGraph();

                break;


            case 7:

                cout << "Enter starting Campaign ID for BFS: ";
                cin >> campaignId;

                graph.BFS(campaignId);

                break;


            case 8:

                cout << "Enter starting Campaign ID for DFS: ";
                cin >> campaignId;

                graph.DFS(campaignId);

                break;


            // =================================
            // EVIDENCE MANAGEMENT
            // =================================

            case 9:

                cout << "\n========== ALL EVIDENCE ==========\n";

                evidenceTree.displayEvidence();

                break;


            case 10:

                cout << "Enter Campaign ID: ";
                cin >> campaignId;

                cout << "\n========== CAMPAIGN EVIDENCE ==========\n";

                evidenceTree.searchByCampaignID(campaignId);

                break;


        case 11:
    riskHeap.displayRanking();
    break;

case 12:
    cout << "Exiting DoubtIt..." << endl;
    break;


            default:

                cout << "Invalid choice. Try again." << endl;
        }

    } while (choice != 12);


    return 0;
}
