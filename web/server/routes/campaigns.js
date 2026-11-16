const express = require('express');
const router  = express.Router();
const db      = require('../db');
const { calculateRiskForCampaign, calculateAllRiskScores } = require('../risk');

// GET /api/campaigns  ?risk=high|medium|low  &sort=score|name|complaints  &page=1&limit=10
router.get('/', (req, res) => {
  const { risk, sort = 'score', page = '1', limit = '10' } = req.query;
  const pageN  = Math.max(1, parseInt(page)  || 1);
  const limitN = Math.min(100, Math.max(1, parseInt(limit) || 10));

  let results = calculateAllRiskScores();

  // Filter by risk level
  if (risk && ['high','medium','low'].includes(risk.toLowerCase())) {
    results = results.filter(r => r.riskLevel.toLowerCase() === risk.toLowerCase());
  }

  // Sort
  if (sort === 'name')       results.sort((a,b) => a.campaign.campaign_name.localeCompare(b.campaign.campaign_name));
  else if (sort === 'complaints') results.sort((a,b) => b.complaintCount - a.complaintCount);
  else                       results.sort((a,b) => b.score - a.score);  // default: score desc

  const total  = results.length;
  const start  = (pageN - 1) * limitN;
  const paged  = results.slice(start, start + limitN);

  res.json({ total, page: pageN, limit: limitN, campaigns: paged });
});

// GET /api/campaigns/ranking  – full ranked list (no pagination)
router.get('/ranking', (req, res) => {
  const results = calculateAllRiskScores().sort((a,b) => b.score - a.score);
  res.json(results);
});

// GET /api/campaigns/:id
router.get('/:id', (req, res) => {
  const campaign = db.getCampaign(req.params.id);
  if (!campaign) return res.status(404).json({ error: 'Campaign not found.' });

  const risk       = calculateRiskForCampaign(req.params.id);
  const complaints = db.getComplaintsForCampaign(req.params.id);
  const evidence   = db.getEvidenceForCampaign(req.params.id);
  const rels       = db.getRelationshipsForCampaign(req.params.id);

  // Build graph connections with campaign names
  const connections = rels.map(r => {
    const otherId = r.source_campaign_id === req.params.id
      ? r.target_campaign_id : r.source_campaign_id;
    const other   = db.getCampaign(otherId);
    return {
      ...r,
      other_campaign_id:   otherId,
      other_campaign_name: other ? other.campaign_name : otherId,
    };
  });

  res.json({ campaign, risk, complaints, evidence, connections });
});

module.exports = router;
