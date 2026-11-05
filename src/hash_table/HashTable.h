
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "../common/Campaign.h"

const int TABLE_SIZE = 101;

struct HashNode {
    Campaign data;
    HashNode* next;

    HashNode(Campaign c) {
        data = c;
        next = nullptr;
    }
};

struct IdentifierNode {
    string identifier;
    Campaign data;
    IdentifierNode* next;

    IdentifierNode(string id, Campaign c) {
        identifier = id;
        data = c;
        next = nullptr;
    }
};

class HashTable {
private:
    HashNode* table[TABLE_SIZE];

    IdentifierNode* upiTable[TABLE_SIZE];
    IdentifierNode* bankTable[TABLE_SIZE];
    IdentifierNode* phoneTable[TABLE_SIZE];
    void searchIdentifier(
    IdentifierNode* identifierTable[],
    string key,
    string type
);

    int hashFunction(string key);

    void insertIdentifier(
        IdentifierNode* identifierTable[],
        string key,
        Campaign c
    );

    void displaySharedIdentifiers(
        IdentifierNode* identifierTable[],
        string type
    );

    void clearIdentifierTable(IdentifierNode* identifierTable[]);
    void removeIdentifier(
    IdentifierNode* identifierTable[],
    string key,
    string campaignId
);

public:
    HashTable();
    ~HashTable();

    void insertCampaign(Campaign c);

    Campaign* searchCampaign(string id);

    bool deleteCampaign(string id);

    void displayTable();

    void displaySharedUPIs();
    void displaySharedBankAccounts();
    void displaySharedPhoneNumbers();
    void searchUPI(string key);
void searchBankAccount(string key);
void searchPhoneNumber(string key);
};

#endif
