const express = require('express');
const router  = express.Router();
const db      = require('../db');

function generateId() {
  return 'CMP-' + Date.now() + '-' + Math.floor(Math.random() * 1000);
}

// GET /api/complaints/:campaignId
router.get('/:campaignId', (req, res) => {
  const complaints = db.getComplaintsForCampaign(req.params.campaignId);
  res.json({ campaign_id: req.params.campaignId, total: complaints.length, complaints });
});

// POST /api/complaints
router.post('/', (req, res) => {
  const { campaign_id, complaint_category, complaint_description, related_identifier } = req.body;

  if (!campaign_id || !campaign_id.trim())
    return res.status(400).json({ error: 'Campaign ID is required.' });
  if (!complaint_category || !complaint_category.trim())
    return res.status(400).json({ error: 'Complaint category is required.' });
  if (!complaint_description || complaint_description.trim().length < 20)
    return res.status(400).json({ error: 'Description must be at least 20 characters.' });

  if (!db.campaignExists(campaign_id.trim()))
    return res.status(404).json({ error: `Campaign "${campaign_id}" not found in database.` });

  const complaint = {
    complaint_id:          generateId(),
    campaign_id:           campaign_id.trim(),
    date_received:         new Date().toISOString().split('T')[0],
    report_channel:        'Web Form',
    complaint_description: complaint_description.trim(),
    complaint_category:    complaint_category.trim(),
    complaint_status:      'Pending Review',
    record_origin:         'User Submitted',
    record_type:           'User',
    related_identifier:    (related_identifier || '').trim(),
  };

  db.addComplaint(complaint);
  res.status(201).json({ success: true, complaint_id: complaint.complaint_id, complaint });
});

module.exports = router;
