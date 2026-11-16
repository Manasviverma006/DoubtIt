#include <iostream>
#include <fstream>
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

// O(n) – appends to the tail (preserves insertion order for CSV-loaded data)
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

// O(1) – prepends to the head (used for runtime-submitted complaints)
void ComplaintList::insertAtFront(Complaint c) {
    ComplaintNode* newNode = new ComplaintNode(c);
    newNode->next = head;
    head = newNode;
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

ComplaintNode* ComplaintList::getHead() const {
    return head;
}

// Appends a single complaint as a CSV row to the given file path.
// Fields that contain commas are wrapped in double quotes.
void ComplaintList::saveComplaintToFile(const Complaint& c,
                                        const string& filepath) {

    ofstream file(filepath, ios::app);

    if (!file.is_open()) {
        cout << "Warning: could not open " << filepath
             << " to save complaint." << endl;
        return;
    }

    // Helper lambda: wrap in quotes if the field contains a comma
    auto q = [](const string& s) -> string {
        if (s.find(',') != string::npos ||
            s.find('"') != string::npos) {
            string out = "\"";
            for (char ch : s) {
                if (ch == '"') out += '"';   // escape embedded quotes
                out += ch;
            }
            out += '"';
            return out;
        }
        return s;
    };

    file << q(c.complaint_id)          << ","
         << q(c.campaign_id)           << ","
         << q(c.date_received)         << ","
         << q(c.report_channel)        << ","
         << q(c.complaint_description) << ","
         << q(c.complaint_category)    << ","
         << q(c.complaint_status)      << ","
         << q(c.record_origin)         << ","
         << q(c.record_type)           << ","
         << q(c.related_identifier)    << "\n";

    file.close();

    cout << "Complaint saved to " << filepath << endl;
}
