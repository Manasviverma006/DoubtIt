
#include <iostream>
#include <fstream>
#include <vector>
#include "common/Campaign.h"
#include "linked_list/CampaignList.h"
#include "hash_table/HashTable.h"
#include <sstream>
#include "common/Complaint.h"
#include "linked_list/ComplaintList.h"

using namespace std;

vector<string> parseCSVLine(string line) {
    vector<string> fields;
    string field;
    bool insideQuotes = false;

    for (int i = 0; i < line.length(); i++) {
        char ch = line[i];

        if (ch == '"') {
            if (insideQuotes && i + 1 < line.length()
                && line[i + 1] == '"') {
                field += '"';
                i++;
            } else {
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


int main() {
    CampaignList list;
    HashTable hashTable;

    ifstream file("data/campaigns.csv");

    if (!file.is_open()) {
        cout << "Unable to open campaigns.csv" << endl;
        return 1;
    }

    string line;
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
    ComplaintList complaintList;

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

cout << "\nCOMPLAINT DATA SUMMARY" << endl;
cout << "Complaints loaded: " << complaintsLoaded << endl;
cout << "Invalid complaint rows skipped: " << complaintsSkipped << endl;

    cout << "\n========== DOUBTIT ==========" << endl;
    cout << "Campaigns loaded: " << loaded << endl;
    cout << "Invalid rows skipped: " << skipped << endl;

   int choice;
string identifier;
string campaignId;

    do {
        cout << "\n========== IDENTIFIER VERIFICATION ==========" << endl;
        cout << "1. Search UPI ID" << endl;
    cout << "2. Search Bank Account" << endl ;
    cout << "3. Search Phone Number" << endl;
    cout << "4. Display Shared Identifiers" << endl;
    cout << "5. Search Complaints by Campaign ID" << endl;
    cout << "6. Exit" << endl;
        cout << "Enter your choice: ";

        cin >> choice;

        switch (choice) {
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

           case 5:
    cout << "Enter Campaign ID: ";
    cin >> campaignId;

    cout << "\nTotal complaints: "
         << complaintList.countComplaintsForCampaign(campaignId)
         << endl;

    complaintList.displayComplaintsForCampaign(campaignId);
    break;

case 6:
    cout << "Exiting DoubtIt..." << endl;
    break;

            default:
                cout << "Invalid choice. Try again." << endl;
        }

    } while (choice != 6);

    return 0;
}
   