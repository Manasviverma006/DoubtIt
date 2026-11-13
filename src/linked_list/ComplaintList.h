#ifndef COMPLAINT_LIST_H
#define COMPLAINT_LIST_H

#include "../common/Complaint.h"

struct ComplaintNode {
    Complaint data;
    ComplaintNode* next;

    ComplaintNode(Complaint c) {
        data = c;
        next = nullptr;
    }
};

class ComplaintList {
private:
    ComplaintNode* head;

public:
    ComplaintList();
    ~ComplaintList();

    void insertComplaint(Complaint c);
    void displayComplaintsForCampaign(string campaignId);
    int countComplaintsForCampaign(string campaignId);
    ComplaintNode* getHead() const;
};

#endif