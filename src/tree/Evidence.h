
#ifndef EVIDENCE_H
#define EVIDENCE_H

#include <string>
using namespace std;

struct Evidence {
    string evidenceID;
    string campaignID;
    string complaintID;
    string evidenceType;
    string description;
    string sourceReference;
    string recordType;
    string verificationStatus;
};

#endif
