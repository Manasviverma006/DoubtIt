
#include "EvidenceTree.h"
#include <iostream>
#include<vector>

using namespace std;

EvidenceTree::EvidenceTree() {
    root = nullptr;
}


EvidenceNode* EvidenceTree::insertNode(EvidenceNode* node, Evidence e) {

    if (node == nullptr) {
        return new EvidenceNode(e);
    }

    if (e.evidenceID < node->data.evidenceID) {
        node->left = insertNode(node->left, e);
    }
    else if (e.evidenceID > node->data.evidenceID) {
        node->right = insertNode(node->right, e);
    }

    return node;
}

void EvidenceTree::insertEvidence(Evidence e) {
    root = insertNode(root, e);
}


void EvidenceTree::inorderTraversal(EvidenceNode* node) {

    if (node == nullptr) {
        return;
    }

    inorderTraversal(node->left);

    cout << "\nEvidence ID: " << node->data.evidenceID;
    cout << "\nCampaign ID: " << node->data.campaignID;
    cout << "\nComplaint ID: " << node->data.complaintID;
    cout << "\nEvidence Type: " << node->data.evidenceType;
    cout << "\nDescription: " << node->data.description;
    cout << "\nVerification: " << node->data.verificationStatus;
    cout << "\n-----------------------------\n";

    inorderTraversal(node->right);
}

void EvidenceTree::displayEvidence() {
    if (root == nullptr) {
        cout << "No evidence records available.\n";
        return;
    }

    inorderTraversal(root);
}


void EvidenceTree::searchByCampaignID(string campaignID) {

    bool found = false;



    vector<EvidenceNode*> nodes;
    nodes.push_back(root);

    while (!nodes.empty()) {

        EvidenceNode* current = nodes.back();
        nodes.pop_back();

        if (current == nullptr) {
            continue;
        }

        if (current->data.campaignID == campaignID) {
            cout << "\nEvidence ID: " << current->data.evidenceID;
            cout << "\nType: " << current->data.evidenceType;
            cout << "\nDescription: " << current->data.description;
            cout << "\nVerification: " << current->data.verificationStatus;
            cout << "\n-----------------------------\n";

            found = true;
        }

        nodes.push_back(current->left);
        nodes.push_back(current->right);
    }

    if (!found) {
        cout << "No evidence found for campaign " << campaignID << "\n";
    }
}

void EvidenceTree::deleteTree(EvidenceNode* node) {

    if (node == nullptr) {
        return;
    }

    deleteTree(node->left);
    deleteTree(node->right);

    delete node;
}

void EvidenceTree::clear() {
    deleteTree(root);
    root = nullptr;
}

EvidenceTree::~EvidenceTree() {
    clear();
}
