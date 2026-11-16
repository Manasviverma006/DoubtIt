
#include "SearchUtils.h"
#include <iostream>
#include <algorithm>
#include <cctype>
using namespace std;


// ============================================================
// HELPER: pretty-print one campaign
// ============================================================

void printCampaignDetail(const Campaign& c) {
    cout << "\n  Campaign ID     : " << c.campaign_id;
    cout << "\n  Campaign Name   : " << c.campaign_name;
    cout << "\n  Organization    : " << c.organization_name;
    cout << "\n  Website         : " << c.website;
    cout << "\n  UPI ID          : " << c.upi_id;
    cout << "\n  Bank Account    : " << c.bank_account_test_id;
    cout << "\n  Phone Number    : " << c.phone_test_id;
    cout << "\n  Record Type     : " << c.record_type;
    cout << "\n  Review Status   : " << c.review_status;
    cout << "\n  ------------------------------------------\n";
}


// ============================================================
// LINEAR SEARCH  –  O(n) substring match (case-insensitive)
// ============================================================

static string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}

int linearSearchByName(const CampaignList& list, const string& query) {

    if (query.empty()) {
        cout << "Search query cannot be empty.\n";
        return 0;
    }

    string lowerQuery = toLower(query);

    CampaignNode* temp = list.getHead();
    int matchCount = 0;

    while (temp != nullptr) {
        string lowerName = toLower(temp->data.campaign_name);

        if (lowerName.find(lowerQuery) != string::npos) {
            matchCount++;
            printCampaignDetail(temp->data);
        }

        temp = temp->next;
    }

    if (matchCount == 0) {
        cout << "No campaigns found matching \"" << query << "\".\n";
    } else {
        cout << "Total matches: " << matchCount << "\n";
    }

    return matchCount;
}


// ============================================================
// BINARY SEARCH  –  O(log n) exact name match on sorted vector
// ============================================================

int binarySearchByName(const vector<Campaign>& sortedVec,
                       const string& targetName) {

    if (sortedVec.empty()) {
        cout << "No campaigns available for binary search.\n";
        return -1;
    }

    int left  = 0;
    int right = static_cast<int>(sortedVec.size()) - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        const string& midName = sortedVec[mid].campaign_name;

        if (midName == targetName) {
            return mid;         // exact match found
        } else if (midName < targetName) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;   // not found
}
