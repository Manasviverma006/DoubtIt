#ifndef COMPLAINT_LIST_H
#define COMPLAINT_LIST_H

#include "../common/Complaint.h"
#include <string>
using namespace std;

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

    void insertComplaint(Complaint c);           // O(n) append to tail
    void insertAtFront(Complaint c);              // O(1) prepend – used for runtime submissions
    void displayComplaintsForCampaign(string campaignId);
    int countComplaintsForCampaign(string campaignId);
    ComplaintNode* getHead() const;

    // Appends the given complaint to complaints.csv on disk
    static void saveComplaintToFile(const Complaint& c, const string& filepath);
};

#endif