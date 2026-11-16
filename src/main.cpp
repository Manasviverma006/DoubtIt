
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

#include "risk/RiskAssessment.h"
#include "risk/MaxHeap.h"

#include "common/Campaign.h"
#include "common/Complaint.h"

#include "linked_list/CampaignList.h"
#include "linked_list/ComplaintList.h"

#include "hash_table/HashTable.h"
#include "graph/graph.h"
#include "tree/EvidenceTree.h"

#include "search/SearchUtils.h"

using namespace std;


// =================================
// CSV PARSING FUNCTION
// =================================

vector<string> parseCSVLine(string line) {

    vector<string> fields;
    string field;
    bool insideQuotes = false;

    for (size_t i = 0; i < line.length(); i++) {

        char ch = line[i];

        if (ch == '"') {

            if (insideQuotes && i + 1 < line.length()
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
// HELPER: current date as string
// =================================

static string currentDateString() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    char buf[12];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
             1900 + ltm->tm_year,
             1 + ltm->tm_mon,
             ltm->tm_mday);
    return string(buf);
}


// =================================
// HELPER: generate complaint ID
// =================================

static string generateComplaintId(int sequence) {
    // Format: CMP-YYYYMMDD-<timestamp_mod1000>-<seq>
    // The timestamp component makes IDs unique across different program runs.
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    char buf[32];
    snprintf(buf, sizeof(buf), "CMP-%04d%02d%02d-%04ld-%03d",
             1900 + ltm->tm_year,
             1 + ltm->tm_mon,
             ltm->tm_mday,
             static_cast<long>(now % 10000),
             sequence % 1000);
    return string(buf);
}


// =================================
// HELPER: print single risk report
// =================================

static void printRiskReportForCampaign(
    const vector<RiskResult>& results,
    const string& campaignId)
{
    for (const RiskResult& r : results) {
        if (r.campaign.campaign_id == campaignId) {
            cout << "\n========== RISK REPORT ==========\n";
            cout << "Campaign ID   : " << r.campaign.campaign_id   << "\n";
            cout << "Campaign Name : " << r.campaign.campaign_name << "\n";
            cout << "Organization  : " << r.campaign.organization_name << "\n";
            cout << "Review Status : " << r.campaign.review_status << "\n";
            cout << "Risk Score    : " << r.score << " / 100\n";
            cout << "Risk Level    : " << r.riskLevel << "\n";
            cout << "Complaints    : " << r.complaintCount << "\n";
            cout << "\nRisk Factors Identified:\n";
            for (const string& reason : r.reasons) {
                cout << "  - " << reason << "\n";
            }
            cout << "\nInterpretation:\n";
            if (r.riskLevel == "HIGH") {
                cout << "  This campaign shows multiple high-risk indicators.\n"
                     << "  Exercise extreme caution before donating.\n";
            } else if (r.riskLevel == "MEDIUM") {
                cout << "  This campaign shows some risk indicators.\n"
                     << "  Verify information independently before donating.\n";
            } else {
                cout << "  No significant risk indicators detected.\n"
                     << "  This is a risk assessment, not a guarantee of legitimacy.\n";
            }
            cout << "=================================\n";
            return;
        }
    }

    cout << "Campaign ID \"" << campaignId
         << "\" not found in risk results.\n";
}


// =================================
// HELPER: save full ranking to file
// =================================

static void saveRiskReportToFile(
    const vector<RiskResult>& results,
    const string& filename)
{
    ofstream out(filename);

    if (!out.is_open()) {
        cout << "Error: could not open \"" << filename
             << "\" for writing.\n";
        return;
    }

    out << "DOUBTIT – CAMPAIGN RISK RANKING REPORT\n";
    out << "Generated: " << currentDateString() << "\n";
    out << "=========================================\n\n";

    // Sort a copy of results by descending score
    vector<RiskResult> sorted = results;
    sort(sorted.begin(), sorted.end(),
         [](const RiskResult& a, const RiskResult& b) {
             return a.score > b.score;
         });

    int rank = 1;
    for (const RiskResult& r : sorted) {
        out << "Rank          : " << rank++ << "\n";
        out << "Campaign ID   : " << r.campaign.campaign_id   << "\n";
        out << "Campaign Name : " << r.campaign.campaign_name << "\n";
        out << "Organization  : " << r.campaign.organization_name << "\n";
        out << "Risk Score    : " << r.score << " / 100\n";
        out << "Risk Level    : " << r.riskLevel << "\n";
        out << "Complaints    : " << r.complaintCount << "\n";
        out << "Risk Factors  :\n";
        for (const string& reason : r.reasons) {
            out << "  - " << reason << "\n";
        }
        out << "-----------------------------------------\n";
    }

    out.close();
    cout << "Risk report saved to \"" << filename << "\".\n";
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

    getline(file, line);   // skip header

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
        c.campaign_id          = fields[0];
        c.campaign_name        = fields[1];
        c.organization_name    = fields[2];
        c.website              = fields[3];
        c.upi_id               = fields[4];
        c.bank_account_test_id = fields[5];
        c.phone_test_id        = fields[6];
        c.record_type          = fields[7];
        c.review_status        = fields[8];

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

    getline(complaintFile, line);   // skip header

    int complaintsLoaded  = 0;
    int complaintsSkipped = 0;

    while (getline(complaintFile, line)) {

        if (line.empty()) continue;

        vector<string> fields = parseCSVLine(line);

        if (fields.size() != 10) {
            complaintsSkipped++;
            continue;
        }

        Complaint c;
        c.complaint_id          = fields[0];
        c.campaign_id           = fields[1];
        c.date_received         = fields[2];
        c.report_channel        = fields[3];
        c.complaint_description = fields[4];
        c.complaint_category    = fields[5];
        c.complaint_status      = fields[6];
        c.record_origin         = fields[7];
        c.record_type           = fields[8];
        c.related_identifier    = fields[9];

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

    getline(evidenceFile, line);   // skip header

    int evidenceLoaded  = 0;
    int evidenceSkipped = 0;

    while (getline(evidenceFile, line)) {

        if (line.empty()) continue;

        vector<string> fields = parseCSVLine(line);

        if (fields.size() != 8) {
            evidenceSkipped++;
            continue;
        }

        Evidence e;
        e.evidenceID         = fields[0];
        e.campaignID         = fields[1];
        e.complaintID        = fields[2];
        e.evidenceType       = fields[3];
        e.description        = fields[4];
        e.sourceReference    = fields[5];
        e.recordType         = fields[6];
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

    getline(relationshipFile, line);   // skip header

    int relationshipsLoaded  = 0;
    int relationshipsSkipped = 0;

    while (getline(relationshipFile, line)) {

        if (line.empty()) continue;

        vector<string> fields = parseCSVLine(line);

        if (fields.size() != 7) {
            relationshipsSkipped++;
            continue;
        }

        string source            = fields[1];
        string target            = fields[2];
        string relationshipType  = fields[3];
        string sharedIdentifier  = fields[4];

        if (source.empty() || target.empty()) {
            relationshipsSkipped++;
            continue;
        }

        graph.addRelationship(source, target,
                              relationshipType, sharedIdentifier);
        relationshipsLoaded++;
    }

    relationshipFile.close();


    // =================================
    // DATA SUMMARY
    // =================================

    cout << "\n========== DOUBTIT ==========\n";

    cout << "\nCAMPAIGN DATA SUMMARY\n";
    cout << "Campaigns loaded            : " << loaded       << "\n";
    cout << "Invalid campaign rows skipped: " << skipped     << "\n";

    cout << "\nCOMPLAINT DATA SUMMARY\n";
    cout << "Complaints loaded           : " << complaintsLoaded   << "\n";
    cout << "Invalid complaint rows skipped: " << complaintsSkipped << "\n";

    cout << "\nEVIDENCE DATA SUMMARY\n";
    cout << "Evidence records loaded     : " << evidenceLoaded   << "\n";
    cout << "Invalid evidence rows skipped: " << evidenceSkipped  << "\n";

    cout << "\nGRAPH DATA SUMMARY\n";
    cout << "Relationships loaded        : " << relationshipsLoaded   << "\n";
    cout << "Invalid relationship rows skipped: " << relationshipsSkipped << "\n";


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
         << " campaigns.\n";


    // =================================
    // RUNTIME STATE
    // =================================

    int runtimeComplaintSeq = 1;   // sequence counter for generated IDs

    // =================================
    // MAIN MENU
    // =================================

    int choice;
    string identifier;
    string campaignId;

    do {

        cout << "\n========== DOUBTIT MENU ==========\n";

        cout << "\n--- IDENTIFIER VERIFICATION ---\n";
        cout << " 1.  Search UPI ID\n";
        cout << " 2.  Search Bank Account\n";
        cout << " 3.  Search Phone Number\n";
        cout << " 4.  Search Website\n";
        cout << " 5.  Display All Shared Identifiers\n";

        cout << "\n--- COMPLAINT MANAGEMENT ---\n";
        cout << " 6.  Search Complaints by Campaign ID\n";
        cout << " 7.  Submit New Complaint\n";

        cout << "\n--- GRAPH OPERATIONS ---\n";
        cout << " 8.  Display Relationship Graph\n";
        cout << " 9.  BFS Traversal (full)\n";
        cout << "10.  DFS Traversal (full)\n";
        cout << "11.  BFS Within N Hops\n";

        cout << "\n--- EVIDENCE MANAGEMENT ---\n";
        cout << "12.  Display All Evidence\n";
        cout << "13.  Search Evidence by Campaign ID\n";

        cout << "\n--- RISK ASSESSMENT ---\n";
        cout << "14.  Display Campaign Risk Ranking\n";
        cout << "15.  View Risk Report for One Campaign\n";
        cout << "16.  Save Risk Report to File\n";

        cout << "\n--- CAMPAIGN MANAGEMENT ---\n";
        cout << "17.  Display All Campaigns\n";
        cout << "18.  Search Campaign by Name (Linear Search)\n";
        cout << "19.  Search Campaign by Name (Binary Search)\n";
        cout << "20.  Sort and Display Campaigns by Name (Merge Sort)\n";

        cout << "\n21.  Exit\n";

        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Enter a number (1-21).\n";
            continue;
        }

        cin.ignore(10000, '\n');   // flush remainder of input line

        switch (choice) {

            // =================================
            // IDENTIFIER VERIFICATION
            // =================================

            case 1:
                cout << "Enter UPI ID: ";
                getline(cin, identifier);
                hashTable.searchUPI(identifier);
                break;

            case 2:
                cout << "Enter Bank Account ID: ";
                getline(cin, identifier);
                hashTable.searchBankAccount(identifier);
                break;

            case 3:
                cout << "Enter Phone Number ID: ";
                getline(cin, identifier);
                hashTable.searchPhoneNumber(identifier);
                break;

            case 4:
                cout << "Enter Website URL: ";
                getline(cin, identifier);
                hashTable.searchWebsite(identifier);
                break;

            case 5:
                hashTable.displaySharedUPIs();
                hashTable.displaySharedBankAccounts();
                hashTable.displaySharedPhoneNumbers();
                hashTable.displaySharedWebsites();
                break;


            // =================================
            // COMPLAINT MANAGEMENT
            // =================================

            case 6:
                cout << "Enter Campaign ID: ";
                getline(cin, campaignId);
                cout << "\nTotal complaints: "
                     << complaintList.countComplaintsForCampaign(campaignId)
                     << "\n";
                complaintList.displayComplaintsForCampaign(campaignId);
                break;

            case 7: {
                // Submit a new complaint at runtime

                Complaint nc;

                cout << "\n--- SUBMIT NEW COMPLAINT ---\n";

                cout << "Enter Campaign ID           : ";
                getline(cin, nc.campaign_id);

                // Validate campaign exists in hash table
                if (hashTable.searchCampaign(nc.campaign_id) == nullptr) {
                    cout << "Warning: Campaign ID \""
                         << nc.campaign_id
                         << "\" was not found in the loaded dataset.\n"
                         << "The complaint will still be recorded.\n";
                }

                cout << "Enter Complaint Category\n"
                     << "(e.g. Fake Campaign / Missing Funds / Misrepresentation): ";
                getline(cin, nc.complaint_category);

                cout << "Enter Complaint Description : ";
                getline(cin, nc.complaint_description);

                cout << "Enter Related Identifier\n"
                     << "(UPI ID / website / phone – leave blank if none): ";
                getline(cin, nc.related_identifier);

                // Auto-fill the remaining fields
                nc.complaint_id   = generateComplaintId(runtimeComplaintSeq++);
                nc.date_received  = currentDateString();
                nc.report_channel = "User-Submitted";
                nc.complaint_status = "Pending Review";
                nc.record_origin  = "Runtime";
                nc.record_type    = "User";

                // Insert at front (O(1))
                complaintList.insertAtFront(nc);

                // Persist to disk
                ComplaintList::saveComplaintToFile(nc, "data/complaints.csv");

                cout << "\nComplaint recorded.\n"
                     << "Complaint ID : " << nc.complaint_id << "\n"
                     << "Campaign ID  : " << nc.campaign_id  << "\n"
                     << "Status       : " << nc.complaint_status << "\n";

                break;
            }


            // =================================
            // GRAPH OPERATIONS
            // =================================

            case 8:
                graph.displayGraph();
                break;

            case 9:
                cout << "Enter starting Campaign ID for BFS: ";
                getline(cin, campaignId);
                graph.BFS(campaignId);
                break;

            case 10:
                cout << "Enter starting Campaign ID for DFS: ";
                getline(cin, campaignId);
                graph.DFS(campaignId);
                break;

            case 11: {
                cout << "Enter starting Campaign ID: ";
                getline(cin, campaignId);

                int maxDepth;
                cout << "Enter maximum number of hops (e.g. 2): ";
                if (!(cin >> maxDepth)) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid depth.\n";
                    break;
                }
                cin.ignore(10000, '\n');

                graph.BFSWithDepth(campaignId, maxDepth);
                break;
            }


            // =================================
            // EVIDENCE MANAGEMENT
            // =================================

            case 12:
                cout << "\n========== ALL EVIDENCE ==========\n";
                evidenceTree.displayEvidence();
                break;

            case 13:
                cout << "Enter Campaign ID: ";
                getline(cin, campaignId);
                cout << "\n========== CAMPAIGN EVIDENCE ==========\n";
                evidenceTree.searchByCampaignID(campaignId);
                break;


            // =================================
            // RISK ASSESSMENT
            // =================================

            case 14:
                riskHeap.displayRanking();
                break;

            case 15:
                cout << "Enter Campaign ID: ";
                getline(cin, campaignId);
                printRiskReportForCampaign(riskResults, campaignId);
                break;

            case 16: {
                string filename = "data/risk_report.txt";
                saveRiskReportToFile(riskResults, filename);
                break;
            }


            // =================================
            // CAMPAIGN MANAGEMENT
            // =================================

            case 17:
                cout << "\n========== ALL CAMPAIGNS ==========\n";
                list.displayCampaigns();
                break;

            case 18: {
                // Linear Search  –  O(n) substring match
                cout << "Enter campaign name (or part of it) to search: ";
                string query;
                getline(cin, query);

                cout << "\n--- Linear Search Results (O(n)) ---\n";
                linearSearchByName(list, query);
                break;
            }

            case 19: {
                // Binary Search  –  O(log n) exact match on sorted vector
                cout << "Enter exact campaign name for binary search: ";
                string query;
                getline(cin, query);

                vector<Campaign> sortedVec = list.getCampaignVector();

                cout << "\n--- Binary Search Result (O(log n)) ---\n";
                int idx = binarySearchByName(sortedVec, query);

                if (idx == -1) {
                    cout << "Campaign \"" << query
                         << "\" not found.\n";
                } else {
                    cout << "Campaign found at sorted index " << idx << ":\n";
                    printCampaignDetail(sortedVec[idx]);
                }
                break;
            }

            case 20: {
                // Merge Sort on the linked list, then display
                cout << "\nSorting campaigns by name using Merge Sort...\n";
                list.sortByName();
                cout << "\n========== CAMPAIGNS (SORTED BY NAME) ==========\n";
                list.displayCampaigns();
                cout << "\nNote: the in-memory list is now permanently sorted.\n";
                break;
            }


            case 21:
                cout << "Exiting DoubtIt. Stay informed, stay safe.\n";
                break;


            default:
                cout << "Invalid choice. Enter a number between 1 and 21.\n";
        }

    } while (choice != 21);

    return 0;
}
