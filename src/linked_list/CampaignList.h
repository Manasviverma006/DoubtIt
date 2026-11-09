
#ifndef CAMPAIGN_LIST_H
#define CAMPAIGN_LIST_H

#include "../common/Campaign.h"

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
};

#endif
