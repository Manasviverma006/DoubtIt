#ifndef COMPLAINT_H
#define COMPLAINT_H

#include <string>
using namespace std;

struct Complaint {
    string complaint_id;
    string campaign_id;
    string date_received;
    string report_channel;
    string complaint_description;
    string complaint_category;
    string complaint_status;
    string record_origin;
    string record_type;
    string related_identifier;
};

#endif