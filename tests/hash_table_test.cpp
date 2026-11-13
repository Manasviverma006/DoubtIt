#include <iostream>
#include "../src/common/Campaign.h"
#include "../src/hash_table/HashTable.h"

using namespace std;

int main() {
    HashTable ht;

    Campaign c;

    c.campaign_id = "C999";
    c.campaign_name = "Test Campaign";
    c.upi_id = "test@upi.test";
    c.bank_account_test_id = "ACC-999";
    c.phone_test_id = "PH-999";

    ht.insertCampaign(c);

    cout << "Before deletion:" << endl;
    ht.searchUPI("test@upi.test");

    cout << "\nDeleting campaign C999..." << endl;

    if (ht.deleteCampaign("C999")) {
        cout << "Campaign deleted successfully." << endl;
    } else {
        cout << "Campaign not found." << endl;
    }

    cout << "\nAfter deletion:" << endl;
    ht.searchUPI("test@upi.test");
    cout << "\nDeleting nonexistent campaign C888..." << endl;

if (ht.deleteCampaign("C888")) {
    cout << "Campaign deleted successfully." << endl;
} else {
    cout << "Campaign not found." << endl;
}

    return 0;
}