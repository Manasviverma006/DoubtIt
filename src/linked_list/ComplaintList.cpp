#include <iostream>
#include "ComplaintList.h"

using namespace std;

ComplaintList::ComplaintList() {
    head = nullptr;
}

ComplaintList::~ComplaintList() {
    ComplaintNode* temp = head;

    while (temp != nullptr) {
        ComplaintNode* nextNode = temp->next;
        delete temp;
        temp = nextNode;
    }
}

void ComplaintList::insertComplaint(Complaint c) {
    ComplaintNode* newNode = new ComplaintNode(c);

    if (head == nullptr) {
        head = newNode;
        return;
    }

    ComplaintNode* temp = head;

    while (temp->next != nullptr) {
        temp = temp->next;
    }

    temp->next = newNode;
}

void ComplaintList::displayComplaintsForCampaign(string campaignId) {
    ComplaintNode* temp = head;
    bool found = false;

    while (temp != nullptr) {
        if (temp->data.campaign_id == campaignId) {
            found = true;

            cout << "\nComplaint ID: "
                 << temp->data.complaint_id << endl;

            cout << "Category: "
                 << temp->data.complaint_category << endl;

            cout << "Description: "
                 << temp->data.complaint_description << endl;

            cout << "Status: "
                 << temp->data.complaint_status << endl;

            cout << "Related Identifier: "
                 << temp->data.related_identifier << endl;
        }

        temp = temp->next;
    }

    if (!found) {
        cout << "No complaints found for this campaign." << endl;
    }
}

int ComplaintList::countComplaintsForCampaign(string campaignId) {
    int count = 0;
    ComplaintNode* temp = head;

    while (temp != nullptr) {
        if (temp->data.campaign_id == campaignId) {
            count++;
        }

        temp = temp->next;
    }

    return count;
}