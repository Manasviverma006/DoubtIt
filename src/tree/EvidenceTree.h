
#ifndef EVIDENCE_TREE_H
#define EVIDENCE_TREE_H

#include "Evidence.h"
#include <string>
using namespace std;

struct EvidenceNode {
    Evidence data;
    EvidenceNode* left;
    EvidenceNode* right;

    EvidenceNode(Evidence e) {
        data = e;
        left = nullptr;
        right = nullptr;
    }
};

class EvidenceTree {
private:
    EvidenceNode* root;

    EvidenceNode* insertNode(EvidenceNode* node, Evidence e);
    void inorderTraversal(EvidenceNode* node);
    void deleteTree(EvidenceNode* node);

public:
    EvidenceTree();

    void insertEvidence(Evidence e);
    void displayEvidence();
    void searchByCampaignID(string campaignID);
    void clear();
    ~EvidenceTree();
};

#endif
