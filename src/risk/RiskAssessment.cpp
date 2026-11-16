
#include "RiskAssessment.h"

#include <algorithm>
#include <cctype>

using namespace std;

// Convert text to lowercase
string toLowerCase(string text) {
    transform(text.begin(), text.end(), text.begin(),
        [](unsigned char c) {
            return static_cast<char>(tolower(c));
        });

    return text;
}

// Assign risk category
string getRiskLevel(int score) {
    if (score >= 50)
        return "HIGH";

    if (score >= 25)
        return "MEDIUM";

    return "LOW";
}

// Calculate risk score for all campaigns
vector<RiskResult> calculateRiskScores(
    const CampaignList& campaigns,
    const ComplaintList& complaints
) {
    vector<RiskResult> results;

    CampaignNode* current = campaigns.getHead();

    while (current != nullptr) {

        Campaign c = current->data;

        RiskResult result;
        result.campaign = c;
        result.score = 0;
        result.complaintCount = 0;

        // 1. Count complaints for this campaign
        ComplaintNode* complaint = complaints.getHead();

        while (complaint != nullptr) {

            if (complaint->data.campaign_id == c.campaign_id) {
                result.complaintCount++;
            }

            complaint = complaint->next;
        }

        int complaintPoints =
            min(result.complaintCount * 3, 15);

        if (complaintPoints > 0) {
            result.score += complaintPoints;

            result.reasons.push_back(
                "Complaint records: " +
                to_string(result.complaintCount)
            );
        }

        // 2. Check shared identifiers
        int upiCount = 0;
        int bankCount = 0;
        int phoneCount = 0;

        CampaignNode* other = campaigns.getHead();

        while (other != nullptr) {

            if (other->data.campaign_id != c.campaign_id) {

                if (!c.upi_id.empty() &&
                    c.upi_id == other->data.upi_id) {
                    upiCount++;
                }

                if (!c.bank_account_test_id.empty() &&
                    c.bank_account_test_id ==
                    other->data.bank_account_test_id) {
                    bankCount++;
                }

                if (!c.phone_test_id.empty() &&
                    c.phone_test_id == other->data.phone_test_id) {
                    phoneCount++;
                }
            }

            other = other->next;
        }

        if (upiCount > 0) {
            result.score += 20;
            result.reasons.push_back("UPI ID shared with another campaign");
        }

        if (bankCount > 0) {
            result.score += 20;
            result.reasons.push_back("Bank account identifier shared");
        }

        if (phoneCount > 0) {
            result.score += 10;
            result.reasons.push_back("Phone number shared with another campaign");
        }

        // 3. Check campaign review status
        string status = toLowerCase(c.review_status);

        if (status.find("flagged") != string::npos ||
            status.find("rejected") != string::npos ||
            status.find("suspended") != string::npos) {

            result.score += 10;
            result.reasons.push_back(
                "Campaign has a flagged, rejected or suspended status"
            );
        }

        // 4. Keep score between 0 and 100
        result.score = min(result.score, 100);

        // 5. Assign risk level
        result.riskLevel = getRiskLevel(result.score);

        if (result.reasons.empty()) {
            result.reasons.push_back(
                "No risk indicators detected by the current rules"
            );
        }

        results.push_back(result);

        current = current->next;
    }

    return results;
}
