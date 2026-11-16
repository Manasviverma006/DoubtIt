
#ifndef SEARCH_UTILS_H
#define SEARCH_UTILS_H

#include "../linked_list/CampaignList.h"
#include <vector>
#include <string>
using namespace std;

// ============================================================
// SEARCHING UTILITIES
// ============================================================

// Linear Search: O(n)
// Traverses the linked list and prints every campaign whose
// name contains 'query' (case-insensitive substring match).
// Returns the count of matches found.
int linearSearchByName(const CampaignList& list, const string& query);

// Binary Search: O(log n)
// Requires a sorted vector<Campaign> (obtain via getCampaignVector()).
// Searches by exact campaign_name match.
// Returns the index of the match, or -1 if not found.
int binarySearchByName(const vector<Campaign>& sortedVec,
                       const string& targetName);

// Pretty-print a Campaign record
void printCampaignDetail(const Campaign& c);

#endif
