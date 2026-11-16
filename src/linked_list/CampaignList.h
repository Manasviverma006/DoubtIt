
#ifndef CAMPAIGN_LIST_H
#define CAMPAIGN_LIST_H

#include "../common/Campaign.h"
#include <vector>

struct CampaignNode {
    Campaign data;
    CampaignNode* next;

    CampaignNode(Campaign c) {
        data = c;
        next = nullptr;
    }
};

class CampaignList {
private:
    CampaignNode* head;

public:
    CampaignList();
    ~CampaignList();

    void insertCampaign(Campaign c);
    void displayCampaigns();
    bool searchCampaign(string id);
    bool deleteCampaign(string id);
    int countCampaigns();
    CampaignNode* getHead() const;

    // Merge sort: sorts the linked list in-place by campaign_name
    void sortByName();

    // Returns all campaigns as a sorted vector (used by binary search)
    vector<Campaign> getCampaignVector() const;
    
};

#endif
