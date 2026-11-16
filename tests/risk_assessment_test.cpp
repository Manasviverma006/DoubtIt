
#include <iostream>
#include <cassert>
#include "../src/risk/RiskAssessment.h"

using namespace std;

int main() {

    CampaignList campaigns;
    ComplaintList complaints;

    // Campaign 1
    Campaign c1;
    c1.campaign_id = "CAM001";
    c1.upi_id = "shared@upi";
    c1.bank_account_test_id = "BANK001";
    c1.phone_test_id = "1111111111";
    c1.review_status = "Flagged";

    // Campaign 2
    Campaign c2;
    c2.campaign_id = "CAM002";
    c2.upi_id = "shared@upi";
    c2.bank_account_test_id = "BANK002";
    c2.phone_test_id = "2222222222";
    c2.review_status = "Approved";

    // Campaign 3
    Campaign c3;
    c3.campaign_id = "CAM003";
    c3.upi_id = "other@upi";
    c3.bank_account_test_id = "BANK002";
    c3.phone_test_id = "2222222222";
    c3.review_status = "Approved";

    campaigns.insertCampaign(c1);
    campaigns.insertCampaign(c2);
    campaigns.insertCampaign(c3);

    // Add complaints
    Complaint cp1;
    cp1.complaint_id = "C001";
    cp1.campaign_id = "CAM001";

    Complaint cp2;
    cp2.complaint_id = "C002";
    cp2.campaign_id = "CAM001";

    Complaint cp3;
    cp3.complaint_id = "C003";
    cp3.campaign_id = "CAM002";

    complaints.insertComplaint(cp1);
    complaints.insertComplaint(cp2);
    complaints.insertComplaint(cp3);

    // Calculate risk
    vector<RiskResult> results =
        calculateRiskScores(campaigns, complaints);

    assert(results.size() == 3);

    // CAM001: 6 complaint + 20 shared UPI + 10 flagged = 36
    assert(results[0].score == 36);
    assert(results[0].riskLevel == "MEDIUM");
    assert(results[0].complaintCount == 2);

    // CAM002: 3 complaint + 20 UPI + 20 bank + 10 phone = 53
    assert(results[1].score == 53);
    assert(results[1].riskLevel == "HIGH");
    assert(results[1].complaintCount == 1);

    // CAM003: 20 bank + 10 phone = 30
    assert(results[2].score == 30);
    assert(results[2].riskLevel == "MEDIUM");
    assert(results[2].complaintCount == 0);

    cout << "All RiskAssessment tests passed!" << endl;

    for (const RiskResult& r : results) {
        cout << "\nCampaign: " << r.campaign.campaign_id;
        cout << "\nScore: " << r.score;
        cout << "\nRisk: " << r.riskLevel;
        cout << "\nComplaints: " << r.complaintCount;
        cout << "\nReasons:";

        for (const string& reason : r.reasons) {
            cout << "\n- " << reason;
        }

        cout << "\n";
    }

    return 0;
}
