
#include <iostream>
#include "CampaignList.h"

using namespace std;

CampaignList::CampaignList() {
    head = nullptr;
}

CampaignList::~CampaignList() {
    CampaignNode* temp = head;

    while (temp != nullptr) {
        CampaignNode* nextNode = temp->next;
        delete temp;
        temp = nextNode;
    }
}

void CampaignList::insertCampaign(Campaign c) {
    CampaignNode* newNode = new CampaignNode(c);

    if (head == nullptr) {
        head = newNode;
        return;
    }

    CampaignNode* temp = head;

    while (temp->next != nullptr) {
        temp = temp->next;
    }

    temp->next = newNode;
}

void CampaignList::displayCampaigns() {
    CampaignNode* temp = head;

    if (head == nullptr) {
        cout << "No campaigns available." << endl;
        return;
    }

    while (temp != nullptr) {
        cout << "Campaign ID: " << temp->data.campaign_id << endl;
        cout << "Campaign Name: " << temp->data.campaign_name << endl;
        cout << "Organization: " << temp->data.organization_name << endl;
        cout << "Review Status: " << temp->data.review_status << endl;
        cout << "------------------------" << endl;

        temp = temp->next;
    }
}

bool CampaignList::searchCampaign(string id) {
    CampaignNode* temp = head;

    while (temp != nullptr) {
        if (temp->data.campaign_id == id) {
            cout << "Campaign found!" << endl;
            cout << "Name: " << temp->data.campaign_name << endl;
            return true;
        }

        temp = temp->next;
    }

    cout << "Campaign not found." << endl;
    return false;
}

bool CampaignList::deleteCampaign(string id) {
    if (head == nullptr) {
        return false;
    }

    if (head->data.campaign_id == id) {
        CampaignNode* temp = head;
        head = head->next;
        delete temp;
        return true;
    }

    CampaignNode* temp = head;

    while (temp->next != nullptr &&
           temp->next->data.campaign_id != id) {
        temp = temp->next;
    }

    if (temp->next == nullptr) {
        return false;
    }

    CampaignNode* nodeToDelete = temp->next;
    temp->next = nodeToDelete->next;
    delete nodeToDelete;

    return true;
}

int CampaignList::countCampaigns() {
    int count = 0;
    CampaignNode* temp = head;

    while (temp != nullptr) {
        count++;
        temp = temp->next;
    }

    return count;
}
