const express = require('express');
const router  = express.Router();
const db      = require('../db');
const { calculateRiskForCampaign } = require('../risk');

// GET /api/search?q=...&type=all|id|name|org|upi|bank|phone|website
router.get('/', (req, res) => {
  const { q = '', type = 'all' } = req.query;
  const query = q.trim();

  if (query.length < 2) {
    return res.status(400).json({ error: 'Search query must be at least 2 characters.' });
  }

  const matches = db.searchByIdentifier(type, query);

  const results = matches.map(c => {
    const risk = calculateRiskForCampaign(c.campaign_id);
    return { campaign: c, risk };
  });

  // Sort by risk score descending
  results.sort((a,b) => b.risk.score - a.risk.score);

  res.json({ query, type, total: results.length, results });
});

module.exports = router;
