
#ifndef RISK_ASSESSMENT_H
#define RISK_ASSESSMENT_H

#include "../linked_list/CampaignList.h"
#include "../linked_list/ComplaintList.h"

#include <vector>
#include <string>

using namespace std;

struct RiskResult {
    Campaign campaign;
    int score;
    string riskLevel;
    int complaintCount;
    vector<string> reasons;
};

vector<RiskResult> calculateRiskScores(
    const CampaignList& campaigns,
    const ComplaintList& complaints
);

#endif
