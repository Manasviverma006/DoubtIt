#include <iostream>
#include "../src/common/Campaign.h"
#include "../src/linked_list/CampaignList.h"

using namespace std;

int main() {
    CampaignList list;

    Campaign c1;
    c1.campaign_id = "C101";
    c1.campaign_name = "Medical Support";

    Campaign c2;
    c2.campaign_id = "C102";
    c2.campaign_name = "Education Support";

    list.insertCampaign(c1);
    list.insertCampaign(c2);

    cout << "Total campaigns: " << list.countCampaigns() << endl;

    cout << "\nSearching for C101:" << endl;
    list.searchCampaign("C101");

    cout << "\nDeleting C101:" << endl;

    if (list.deleteCampaign("C101")) {
        cout << "Campaign deleted successfully." << endl;
    } else {
        cout << "Campaign not found." << endl;
    }

    cout << "\nTotal campaigns after deletion: "
         << list.countCampaigns() << endl;

    cout << "\nSearching for deleted C101:" << endl;
    list.searchCampaign("C101");

    return 0;
}