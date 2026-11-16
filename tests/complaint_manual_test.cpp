#include <iostream>
#include <cassert>
#include "../src/linked_list/ComplaintList.h"

using namespace std;

int main() {

    ComplaintList complaints;

    Complaint c1;
    c1.complaint_id = "C001";
    c1.campaign_id = "CAM001";
    c1.complaint_category = "Fraud";

    Complaint c2;
    c2.complaint_id = "C002";
    c2.campaign_id = "CAM001";
    c2.complaint_category = "Misuse";

    Complaint c3;
    c3.complaint_id = "C003";
    c3.campaign_id = "CAM002";
    c3.complaint_category = "Fraud";

    complaints.insertComplaint(c1);
    complaints.insertComplaint(c2);
    complaints.insertComplaint(c3);

    assert(complaints.countComplaintsForCampaign("CAM001") == 2);
    assert(complaints.countComplaintsForCampaign("CAM002") == 1);
    assert(complaints.countComplaintsForCampaign("CAM999") == 0);

    cout << "ComplaintList tests passed!" << endl;

    complaints.displayComplaintsForCampaign("CAM001");

    return 0;
}