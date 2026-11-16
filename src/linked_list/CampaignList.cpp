
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
CampaignNode* CampaignList::getHead() const {
    return head;
}


// ============================================================
// MERGE SORT HELPERS (operate on CampaignNode* pointers)
// ============================================================

// Split list into two halves using the slow/fast pointer technique
static void splitList(CampaignNode* source,
                      CampaignNode** frontRef,
                      CampaignNode** backRef) {

    if (source == nullptr || source->next == nullptr) {
        *frontRef = source;
        *backRef = nullptr;
        return;
    }

    CampaignNode* slow = source;
    CampaignNode* fast = source->next;

    while (fast != nullptr) {
        fast = fast->next;
        if (fast != nullptr) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *frontRef = source;
    *backRef = slow->next;
    slow->next = nullptr;   // cut the list at mid-point
}

// Merge two sorted sublists and return the merged head
static CampaignNode* mergeSorted(CampaignNode* a, CampaignNode* b) {

    if (a == nullptr) return b;
    if (b == nullptr) return a;

    CampaignNode* result;

    if (a->data.campaign_name <= b->data.campaign_name) {
        result = a;
        result->next = mergeSorted(a->next, b);
    } else {
        result = b;
        result->next = mergeSorted(a, b->next);
    }

    return result;
}

// Recursive merge sort on node pointer
static void mergeSort(CampaignNode** headRef) {
    CampaignNode* h = *headRef;

    if (h == nullptr || h->next == nullptr)
        return;

    CampaignNode* front = nullptr;
    CampaignNode* back  = nullptr;

    splitList(h, &front, &back);

    mergeSort(&front);
    mergeSort(&back);

    *headRef = mergeSorted(front, back);
}

void CampaignList::sortByName() {
    mergeSort(&head);
}

// Return a snapshot of all campaigns as a sorted std::vector
// (sort is by name using std::sort on the vector, not on the list)
#include <algorithm>

vector<Campaign> CampaignList::getCampaignVector() const {

    vector<Campaign> vec;
    CampaignNode* temp = head;

    while (temp != nullptr) {
        vec.push_back(temp->data);
        temp = temp->next;
    }

    sort(vec.begin(), vec.end(),
         [](const Campaign& a, const Campaign& b) {
             return a.campaign_name < b.campaign_name;
         });

    return vec;
}
