/**
 * db.js – Pure-JS data layer
 * In-memory Maps for fast reads (from CSV)
 * JSON file persistence for new writes (submissions, reviews, admins)
 */

const fs   = require('fs');
const path = require('path');
const bcrypt = require('bcryptjs');

const DATA_DIR  = path.join(__dirname, '../../data');
const STORE_DIR = path.join(__dirname, '../data');

if (!fs.existsSync(STORE_DIR)) fs.mkdirSync(STORE_DIR, { recursive: true });

// ── CSV PARSER ────────────────────────────────────────────────
function parseCSVLine(line) {
  const fields = []; let field = ''; let inQ = false;
  for (let i = 0; i < line.length; i++) {
    const ch = line[i];
    if (ch === '"') {
      if (inQ && line[i+1] === '"') { field += '"'; i++; }
      else inQ = !inQ;
    } else if (ch === ',' && !inQ) { fields.push(field); field = ''; }
    else field += ch;
  }
  fields.push(field);
  return fields;
}

function parseCSV(file) {
  const fp = path.join(DATA_DIR, file);
  if (!fs.existsSync(fp)) return [];
  const lines = fs.readFileSync(fp, 'utf8').split('\n');
  if (lines.length < 2) return [];
  const headers = parseCSVLine(lines[0]).map(h => h.trim());
  const rows = [];
  for (let i = 1; i < lines.length; i++) {
    const line = lines[i].trim();
    if (!line) continue;
    const vals = parseCSVLine(line);
    if (vals.length !== headers.length) continue;
    const row = {};
    headers.forEach((h, j) => { row[h] = vals[j]; });
    rows.push(row);
  }
  return rows;
}

// ── JSON STORE ────────────────────────────────────────────────
function readStore(name) {
  const fp = path.join(STORE_DIR, name + '.json');
  if (!fs.existsSync(fp)) return [];
  try { return JSON.parse(fs.readFileSync(fp, 'utf8')); }
  catch { return []; }
}

function writeStore(name, data) {
  const fp = path.join(STORE_DIR, name + '.json');
  fs.writeFileSync(fp, JSON.stringify(data, null, 2), 'utf8');
}

// ── LOAD CAMPAIGN DATA ────────────────────────────────────────
const _campaignsRaw  = parseCSV('campaigns.csv');
const _complaintsRaw = parseCSV('complaints.csv');
const _evidenceRaw   = parseCSV('evidence.csv');
const _relsRaw       = parseCSV('relationships.csv');

// Map: campaign_id → campaign object
const campaignsMap = new Map();
_campaignsRaw.forEach(r => {
  campaignsMap.set(r.campaign_id, {
    campaign_id:       r.campaign_id,
    campaign_name:     r.campaign_name,
    organization_name: r.organization_name,
    website:           r.website,
    upi_id:            r.upi_id,
    bank_account_id:   r.bank_account_test_id,
    phone_id:          r.phone_test_id,
    record_type:       r.record_type,
    review_status:     r.review_status,
    verified_status:   'Under Review',
    source:            'Dataset',
  });
});

const complaintsArr  = _complaintsRaw.map(r => ({
  complaint_id:          r.complaint_id,
  campaign_id:           r.campaign_id,
  date_received:         r.date_received,
  report_channel:        r.report_channel,
  complaint_description: r.complaint_description,
  complaint_category:    r.complaint_category,
  complaint_status:      r.complaint_status,
  record_origin:         r.record_origin,
  record_type:           r.record_type,
  related_identifier:    r.related_identifier,
}));

const evidenceArr = _evidenceRaw.map(r => ({
  evidence_id:          r.evidence_id,
  campaign_id:          r.campaign_id,
  complaint_id:         r.complaint_id,
  evidence_type:        r.evidence_type,
  evidence_description: r.evidence_description,
  source_reference:     r.source_reference,
  record_type:          r.record_type,
  verification_status:  r.verification_status,
}));

const relationshipsArr = _relsRaw
  .filter(r => r.relationship_id && r.source_campaign_id)
  .map(r => ({
    relationship_id:    r.relationship_id,
    source_campaign_id: r.source_campaign_id,
    target_campaign_id: r.target_campaign_id,
    relationship_type:  r.relationship_type,
    shared_identifier:  r.shared_identifier,
    record_type:        r.record_type,
    interpretation:     r.interpretation,
  }));

// Add approved submissions to campaignsMap
const _approvedSubs = readStore('approved_campaigns');
_approvedSubs.forEach(c => campaignsMap.set(c.campaign_id, c));

console.log(`[DB] Loaded ${campaignsMap.size} campaigns, ${complaintsArr.length} complaints, ` +
            `${evidenceArr.length} evidence, ${relationshipsArr.length} relationships`);

// ── ADMIN USERS ───────────────────────────────────────────────
let admins = readStore('admins');
if (admins.length === 0) {
  const hash = bcrypt.hashSync('admin123', 10);
  admins = [{ username: 'admin', password_hash: hash, display_name: 'DoubtIt Admin', created_at: new Date().toISOString() }];
  writeStore('admins', admins);
  console.log('[DB] Admin seeded  →  username: admin  |  password: admin123');
}

// ── PUBLIC API ────────────────────────────────────────────────
const db = {

  // CAMPAIGNS
  getAllCampaigns()       { return [...campaignsMap.values()]; },
  getCampaign(id)        { return campaignsMap.get(id) || null; },
  campaignExists(id)     { return campaignsMap.has(id); },

  // COMPLAINTS
  getAllComplaints()         { return complaintsArr; },
  getComplaintsForCampaign(cid) {
    return complaintsArr.filter(c => c.campaign_id === cid);
  },
  addComplaint(complaint) {
    complaintsArr.push(complaint);
  },

  // EVIDENCE
  getEvidenceForCampaign(cid) {
    return evidenceArr.filter(e => e.campaign_id === cid);
  },

  // RELATIONSHIPS
  getRelationshipsForCampaign(cid) {
    return relationshipsArr.filter(
      r => r.source_campaign_id === cid || r.target_campaign_id === cid
    );
  },

  // SHARED IDENTIFIER SEARCH
  searchByIdentifier(type, value) {
    const v = value.toLowerCase();
    const campaigns = db.getAllCampaigns();
    switch (type) {
      case 'upi':     return campaigns.filter(c => c.upi_id && c.upi_id.toLowerCase() === v);
      case 'bank':    return campaigns.filter(c => c.bank_account_id && c.bank_account_id.toLowerCase() === v);
      case 'phone':   return campaigns.filter(c => c.phone_id && c.phone_id.toLowerCase() === v);
      case 'website': return campaigns.filter(c => c.website && c.website.toLowerCase().includes(v));
      case 'name':    return campaigns.filter(c => c.campaign_name && c.campaign_name.toLowerCase().includes(v));
      case 'org':     return campaigns.filter(c => c.organization_name && c.organization_name.toLowerCase().includes(v));
      case 'id':      return campaigns.filter(c => c.campaign_id && c.campaign_id.toLowerCase() === v);
      default:        return campaigns.filter(c =>
        (c.campaign_name && c.campaign_name.toLowerCase().includes(v)) ||
        (c.organization_name && c.organization_name.toLowerCase().includes(v)) ||
        (c.campaign_id && c.campaign_id.toLowerCase() === v) ||
        (c.upi_id && c.upi_id.toLowerCase() === v) ||
        (c.website && c.website.toLowerCase().includes(v))
      );
    }
  },

  // SUBMISSIONS
  getSubmissions()           { return readStore('submissions'); },
  getPendingSubmissions()    { return readStore('submissions').filter(s => s.review_status === 'Pending Review'); },
  getSubmission(id)          { return readStore('submissions').find(s => s.submission_id === id) || null; },
  addSubmission(sub) {
    const list = readStore('submissions');
    list.unshift(sub);
    writeStore('submissions', list);
  },
  updateSubmissionStatus(id, status) {
    const list = readStore('submissions');
    const idx  = list.findIndex(s => s.submission_id === id);
    if (idx !== -1) { list[idx].review_status = status; writeStore('submissions', list); }
  },

  // REVIEW RECORDS
  getReviews()          { return readStore('reviews'); },
  getReviewsFor(subId)  { return readStore('reviews').filter(r => r.submission_id === subId); },
  addReview(review) {
    const list = readStore('reviews');
    list.unshift(review);
    writeStore('reviews', list);
  },

  // APPROVE SUBMISSION → public campaign database
  approveSubmission(sub, reviewerUsername, verifiedStatus) {
    const newId = 'SUB-' + sub.submission_id;
    const campaign = {
      campaign_id:       newId,
      campaign_name:     sub.campaign_name,
      organization_name: sub.organization_name,
      website:           sub.campaign_url || '',
      upi_id:            '',
      bank_account_id:   '',
      phone_id:          '',
      record_type:       'User Submitted',
      review_status:     'Reviewed',
      verified_status:   verifiedStatus,
      source:            'User Submission',
    };
    campaignsMap.set(newId, campaign);
    const approved = readStore('approved_campaigns');
    approved.push(campaign);
    writeStore('approved_campaigns', approved);
    return campaign;
  },

  // ADMIN USERS
  getAdmin(username) { return admins.find(a => a.username === username) || null; },
  verifyAdmin(username, password) {
    const admin = db.getAdmin(username);
    if (!admin) return false;
    return bcrypt.compareSync(password, admin.password_hash);
  },

  // STATS
  getStats() {
    return {
      totalCampaigns:    campaignsMap.size,
      totalComplaints:   complaintsArr.length,
      totalSubmissions:  readStore('submissions').length,
      pendingReview:     readStore('submissions').filter(s => s.review_status === 'Pending Review').length,
    };
  },
};

module.exports = db;
