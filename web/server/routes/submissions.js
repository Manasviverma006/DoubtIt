const express = require('express');
const router  = express.Router();
const db      = require('../db');
const { requireAdmin } = require('../middleware/auth');

function generateSubId() {
  return 'REP-' + Date.now() + '-' + Math.floor(Math.random() * 10000).toString().padStart(4,'0');
}

function generateReviewId() {
  return 'REV-' + Date.now();
}

// VALID_URL – basic URL validation (http/https only)
function isValidUrl(str) {
  try { const u = new URL(str); return u.protocol === 'http:' || u.protocol === 'https:'; }
  catch { return false; }
}

// ── PUBLIC: submit new campaign ──────────────────────────────
// POST /api/submissions
router.post('/', (req, res) => {
  const {
    campaign_name, campaign_url, organization_name,
    campaign_description, campaign_category,
    reason_for_reporting, evidence_links, submitter_contact,
  } = req.body;

  const errors = [];
  if (!campaign_name || campaign_name.trim().length < 3)
    errors.push('Campaign name must be at least 3 characters.');
  if (!organization_name || organization_name.trim().length < 2)
    errors.push('Organisation name is required.');
  if (!campaign_description || campaign_description.trim().length < 50)
    errors.push('Campaign description must be at least 50 characters.');
  if (!reason_for_reporting || reason_for_reporting.trim().length < 30)
    errors.push('Reason for reporting must be at least 30 characters.');
  if (campaign_url && campaign_url.trim() && !isValidUrl(campaign_url.trim()))
    errors.push('Campaign URL must be a valid http/https URL.');
  if (submitter_contact && submitter_contact.trim()) {
    const emailRe = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    if (!emailRe.test(submitter_contact.trim()))
      errors.push('Contact information must be a valid email address.');
  }

  if (errors.length) return res.status(400).json({ errors });

  const submission = {
    submission_id:        generateSubId(),
    campaign_name:        campaign_name.trim(),
    campaign_url:         (campaign_url || '').trim(),
    organization_name:    organization_name.trim(),
    campaign_description: campaign_description.trim(),
    campaign_category:    (campaign_category || 'Other').trim(),
    reason_for_reporting: reason_for_reporting.trim(),
    evidence_links:       (evidence_links || '').trim(),
    submitter_contact:    (submitter_contact || '').trim(),
    submission_date:      new Date().toISOString(),
    review_status:        'Pending Review',
    submitted_ip:         req.ip || '',
  };

  db.addSubmission(submission);
  res.status(201).json({ success: true, submission_id: submission.submission_id });
});

// ── ADMIN ONLY ───────────────────────────────────────────────

// GET /api/submissions  (admin)
router.get('/', requireAdmin, (req, res) => {
  const { status } = req.query;
  let list = db.getSubmissions();
  if (status) list = list.filter(s => s.review_status === status);
  res.json({ total: list.length, submissions: list });
});

// GET /api/submissions/:id  (admin)
router.get('/:id', requireAdmin, (req, res) => {
  const sub = db.getSubmission(req.params.id);
  if (!sub) return res.status(404).json({ error: 'Submission not found.' });
  const reviews = db.getReviewsFor(req.params.id);
  res.json({ submission: sub, reviews });
});

// POST /api/submissions/:id/review  (admin)
router.post('/:id/review', requireAdmin, (req, res) => {
  const { review_decision, review_notes } = req.body;

  const VALID_DECISIONS = [
    'Verified Legitimate', 'Fraudulent', 'Suspicious',
    'Needs More Information', 'Rejected',
  ];

  if (!review_decision || !VALID_DECISIONS.includes(review_decision))
    return res.status(400).json({ error: `Decision must be one of: ${VALID_DECISIONS.join(', ')}` });

  const sub = db.getSubmission(req.params.id);
  if (!sub) return res.status(404).json({ error: 'Submission not found.' });

  const review = {
    review_id:        generateReviewId(),
    submission_id:    req.params.id,
    reviewer_username: req.session.adminUsername,
    review_decision,
    review_notes:     (review_notes || '').trim(),
    review_date:      new Date().toISOString(),
  };

  db.addReview(review);
  db.updateSubmissionStatus(req.params.id, review_decision);

  // If approved (not Rejected / Needs More Info) → add to public campaign db
  let publicCampaign = null;
  if (!['Rejected', 'Needs More Information'].includes(review_decision)) {
    publicCampaign = db.approveSubmission(sub, req.session.adminUsername, review_decision);
  }

  res.json({ success: true, review, publicCampaign });
});

module.exports = router;
