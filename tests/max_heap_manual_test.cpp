#include <iostream>
#include <cassert>
#include "../src/risk/MaxHeap.h"

using namespace std;

int main() {

    MaxHeap heap;

    RiskResult r1;
    r1.campaign.campaign_id = "CAM001";
    r1.campaign.campaign_name = "Campaign One";
    r1.score = 30;
    r1.riskLevel = "Low";

    RiskResult r2;
    r2.campaign.campaign_id = "CAM002";
    r2.campaign.campaign_name = "Campaign Two";
    r2.score = 90;
    r2.riskLevel = "High";

    RiskResult r3;
    r3.campaign.campaign_id = "CAM003";
    r3.campaign.campaign_name = "Campaign Three";
    r3.score = 60;
    r3.riskLevel = "Medium";

    heap.insert(r1);
    heap.insert(r2);
    heap.insert(r3);

    assert(!heap.isEmpty());

    RiskResult result;

    assert(heap.extractMax(result));
    assert(result.score == 90);

    assert(heap.extractMax(result));
    assert(result.score == 60);

    assert(heap.extractMax(result));
    assert(result.score == 30);

    assert(heap.isEmpty());

    assert(!heap.extractMax(result));

    cout << "All MaxHeap tests passed!" << endl;

    return 0;
}