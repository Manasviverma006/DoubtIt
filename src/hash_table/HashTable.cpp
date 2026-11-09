
#include <iostream>
#include "HashTable.h"

using namespace std;

HashTable::HashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i] = nullptr;
        upiTable[i] = nullptr;
        bankTable[i] = nullptr;
        phoneTable[i] = nullptr;
    }
}

HashTable::~HashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* temp = table[i];

        while (temp != nullptr) {
            HashNode* nextNode = temp->next;
            delete temp;
            temp = nextNode;
        }
    }

    clearIdentifierTable(upiTable);
    clearIdentifierTable(bankTable);
    clearIdentifierTable(phoneTable);
}

int HashTable::hashFunction(string key) {
    unsigned long hash = 0;

    for (char ch : key) {
        hash = (hash * 31 + static_cast<unsigned char>(ch)) % TABLE_SIZE;
    }

    return hash;
}

void HashTable::clearIdentifierTable(IdentifierNode* identifierTable[]) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        IdentifierNode* temp = identifierTable[i];

        while (temp != nullptr) {
            IdentifierNode* nextNode = temp->next;
            delete temp;
            temp = nextNode;
        }

        identifierTable[i] = nullptr;
    }
}

void HashTable::insertIdentifier(
    IdentifierNode* identifierTable[],
    string key,
    Campaign c
) {
    if (key.empty()) {
        return;
    }

    int index = hashFunction(key);

    IdentifierNode* newNode = new IdentifierNode(key, c);
    newNode->next = identifierTable[index];
    identifierTable[index] = newNode;
}

void HashTable::removeIdentifier(
    IdentifierNode* identifierTable[],
    string key,
    string campaignId
) {
    if (key.empty()) {
        return;
    }

    int index = hashFunction(key);

    IdentifierNode* temp = identifierTable[index];
    IdentifierNode* previous = nullptr;

    while (temp != nullptr) {
        if (temp->identifier == key &&
            temp->data.campaign_id == campaignId) {

            if (previous == nullptr) {
                identifierTable[index] = temp->next;
            } else {
                previous->next = temp->next;
            }

            delete temp;
            return;
        }

        previous = temp;
        temp = temp->next;
    }
}

void HashTable::insertCampaign(Campaign c) {
    int index = hashFunction(c.campaign_id);

    HashNode* temp = table[index];

    while (temp != nullptr) {
        if (temp->data.campaign_id == c.campaign_id) {
            removeIdentifier(upiTable, temp->data.upi_id, c.campaign_id);
            removeIdentifier(bankTable, temp->data.bank_account_test_id, c.campaign_id);
            removeIdentifier(phoneTable, temp->data.phone_test_id, c.campaign_id);

            temp->data = c;

            insertIdentifier(upiTable, c.upi_id, c);
            insertIdentifier(bankTable, c.bank_account_test_id, c);
            insertIdentifier(phoneTable, c.phone_test_id, c);

            return;
        }

        temp = temp->next;
    }

    HashNode* newNode = new HashNode(c);
    newNode->next = table[index];
    table[index] = newNode;

    insertIdentifier(upiTable, c.upi_id, c);
    insertIdentifier(bankTable, c.bank_account_test_id, c);
    insertIdentifier(phoneTable, c.phone_test_id, c);
}

Campaign* HashTable::searchCampaign(string id) {
    int index = hashFunction(id);

    HashNode* temp = table[index];

    while (temp != nullptr) {
        if (temp->data.campaign_id == id) {
            return &temp->data;
        }

        temp = temp->next;
    }

    return nullptr;
}

bool HashTable::deleteCampaign(string id) {
    int index = hashFunction(id);

    HashNode* temp = table[index];
    HashNode* previous = nullptr;

    while (temp != nullptr) {
        if (temp->data.campaign_id == id) {

            removeIdentifier(upiTable, temp->data.upi_id, id);
            removeIdentifier(bankTable, temp->data.bank_account_test_id, id);
            removeIdentifier(phoneTable, temp->data.phone_test_id, id);

            if (previous == nullptr) {
                table[index] = temp->next;
            } else {
                previous->next = temp->next;
            }

            delete temp;
            return true;
        }

        previous = temp;
        temp = temp->next;
    }

    return false;
}

void HashTable::displayTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* temp = table[i];

        if (temp != nullptr) {
            cout << "Bucket " << i << ": ";

            while (temp != nullptr) {
                cout << temp->data.campaign_id << " -> ";
                temp = temp->next;
            }

            cout << "NULL" << endl;
        }
    }
}

void HashTable::displaySharedIdentifiers(
    IdentifierNode* identifierTable[],
    string type
) {
    cout << "\nShared " << type << " identifiers:\n";

    bool found = false;

    for (int i = 0; i < TABLE_SIZE; i++) {
        IdentifierNode* current = identifierTable[i];

        while (current != nullptr) {
            string key = current->identifier;

            int count = 0;
            IdentifierNode* temp = identifierTable[i];

            while (temp != nullptr) {
                if (temp->identifier == key) {
                    count++;
                }

                temp = temp->next;
            }

            bool alreadyDisplayed = false;
            IdentifierNode* check = identifierTable[i];

            while (check != current) {
                if (check->identifier == key) {
                    alreadyDisplayed = true;
                    break;
                }

                check = check->next;
            }

            if (count > 1 && !alreadyDisplayed) {
                found = true;

                cout << "\nIdentifier: " << key << endl;
                cout << "Used by " << count << " campaigns:" << endl;

                IdentifierNode* campaign = identifierTable[i];

                while (campaign != nullptr) {
                    if (campaign->identifier == key) {
                        cout << campaign->data.campaign_id
                             << " - "
                             << campaign->data.campaign_name
                             << endl;
                    }

                    campaign = campaign->next;
                }
            }

            current = current->next;
        }
    }

    if (!found) {
        cout << "No shared identifiers found." << endl;
    }
}

void HashTable::displaySharedUPIs() {
    displaySharedIdentifiers(upiTable, "UPI");
}

void HashTable::displaySharedBankAccounts() {
    displaySharedIdentifiers(bankTable, "Bank Account");
}

void HashTable::displaySharedPhoneNumbers() {
    displaySharedIdentifiers(phoneTable, "Phone Number");
}

void HashTable::searchIdentifier(
    IdentifierNode* identifierTable[],
    string key,
    string type
) {
    if (key.empty()) {
        cout << "Invalid identifier." << endl;
        return;
    }

    int index = hashFunction(key);

    IdentifierNode* temp = identifierTable[index];
    int count = 0;

    while (temp != nullptr) {
        if (temp->identifier == key) {
            count++;
        }

        temp = temp->next;
    }

    if (count == 0) {
        cout << "\nNo campaign found with this " << type << "." << endl;
        return;
    }

    cout << "\nIdentifier found!" << endl;
    cout << "Type: " << type << endl;
    cout << "Matching campaigns: " << count << endl;

    temp = identifierTable[index];

    while (temp != nullptr) {
        if (temp->identifier == key) {
            cout << temp->data.campaign_id
                 << " - "
                 << temp->data.campaign_name
                 << endl;
        }

        temp = temp->next;
    }

    if (count > 1) {
        cout << "WARNING: This identifier is shared by multiple campaigns."
             << endl;
    } else {
        cout << "No duplicate identifier found in the dataset." << endl;
    }
}

void HashTable::searchUPI(string key) {
    searchIdentifier(upiTable, key, "UPI ID");
}

void HashTable::searchBankAccount(string key) {
    searchIdentifier(bankTable, key, "Bank Account");
}

void HashTable::searchPhoneNumber(string key) {
    searchIdentifier(phoneTable, key, "Phone Number");
}
