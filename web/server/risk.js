/**
 * risk.js – Risk scoring engine (JS port of C++ RiskAssessment)
 * Returns score 0–100, risk level, factors, and complaint count.
 */

const db = require('./db');

const SCORE = {
  COMPLAINT_PER:    5,   // per complaint (capped at 30)
  COMPLAINT_MAX:   30,
  SHARED_UPI:      15,
  SHARED_BANK:     15,
  SHARED_PHONE:    10,
  SHARED_WEBSITE:   5,
  BAD_STATUS:      20,
};

function calculateRiskForCampaign(campaignId) {
  const campaign    = db.getCampaign(campaignId);
  const complaints  = db.getComplaintsForCampaign(campaignId);
  const rels        = db.getRelationshipsForCampaign(campaignId);

  let score = 0;
  const reasons = [];

  // Complaint score
  if (complaints.length > 0) {
    const s = Math.min(complaints.length * SCORE.COMPLAINT_PER, SCORE.COMPLAINT_MAX);
    score += s;
    reasons.push(`Complaint records: ${complaints.length}`);
  }

  // Shared identifiers via relationship edges
  const hasRelType = type => rels.some(r => r.relationship_type === type);

  if (hasRelType('UPI')) {
    score += SCORE.SHARED_UPI;
    reasons.push('UPI ID shared with another campaign');
  }
  if (hasRelType('Bank test ID')) {
    score += SCORE.SHARED_BANK;
    reasons.push('Bank account identifier shared');
  }
  if (hasRelType('Phone test ID')) {
    score += SCORE.SHARED_PHONE;
    reasons.push('Phone number shared with another campaign');
  }

  // Review status check
  if (campaign) {
    const st = (campaign.review_status || '').toLowerCase();
    if (st.includes('flagged') || st.includes('rejected') || st.includes('suspended')) {
      score += SCORE.BAD_STATUS;
      reasons.push('Campaign has a flagged, rejected or suspended status');
    }
  }

  score = Math.min(score, 100);

  return {
    campaign_id:    campaignId,
    score,
    riskLevel:      score >= 50 ? 'HIGH' : score >= 25 ? 'MEDIUM' : 'LOW',
    reasons,
    complaintCount: complaints.length,
  };
}

function calculateAllRiskScores() {
  return db.getAllCampaigns().map(c => ({
    campaign:  c,
    ...calculateRiskForCampaign(c.campaign_id),
  }));
}

module.exports = { calculateRiskForCampaign, calculateAllRiskScores };
