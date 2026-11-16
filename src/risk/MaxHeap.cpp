
#include "MaxHeap.h"
#include <iostream>
#include <utility>

using namespace std;

void MaxHeap::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;

        if (heap[index].score <= heap[parent].score)
            break;

        swap(heap[index], heap[parent]);

        index = parent;
    }
}

void MaxHeap::heapifyDown(int index) {
    int size = static_cast<int>(heap.size());

    while (true) {
        int largest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size &&
            heap[left].score > heap[largest].score) {
            largest = left;
        }

        if (right < size &&
            heap[right].score > heap[largest].score) {
            largest = right;
        }

        if (largest == index)
            break;

        swap(heap[index], heap[largest]);

        index = largest;
    }
}

void MaxHeap::insert(RiskResult result) {
    heap.push_back(result);

    heapifyUp(static_cast<int>(heap.size()) - 1);
}

bool MaxHeap::extractMax(RiskResult& result) {
    if (heap.empty())
        return false;

    result = heap[0];

    if (heap.size() == 1) {
        heap.pop_back();
        return true;
    }

    heap[0] = heap.back();
    heap.pop_back();

    heapifyDown(0);

    return true;
}

bool MaxHeap::isEmpty() const {
    return heap.empty();
}

void MaxHeap::displayRanking() const {
    if (heap.empty()) {
        cout << "No risk results available." << endl;
        return;
    }

    // Use a copy so displaying the ranking
    // does not destroy the original heap.
    MaxHeap temp = *this;

    RiskResult result;
    int rank = 1;

    cout << "\n========== CAMPAIGN RISK RANKING ==========\n";

    while (temp.extractMax(result)) {
        cout << "\nRank: " << rank++ << endl;
        cout << "Campaign ID: "
             << result.campaign.campaign_id << endl;
        cout << "Campaign Name: "
             << result.campaign.campaign_name << endl;
        cout << "Risk Score: "
             << result.score << "/100" << endl;
        cout << "Risk Level: "
             << result.riskLevel << endl;

        cout << "Risk Factors:" << endl;

        if (result.reasons.empty()) {
            cout << "- No risk indicators detected." << endl;
        } else {
            for (const string& reason : result.reasons) {
                cout << "- " << reason << endl;
            }
        }

        cout << "-----------------------------------" << endl;
    }
}
