
#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include "RiskAssessment.h"
#include <vector>

using namespace std;

class MaxHeap {
private:
    vector<RiskResult> heap;

    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    void insert(RiskResult result);
    bool extractMax(RiskResult& result);
    bool isEmpty() const;
    void displayRanking() const;
};

#endif
