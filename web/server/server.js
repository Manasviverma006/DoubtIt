/**
 * server.js – DoubtIt Express server
 */

const express      = require('express');
const session      = require('express-session');
const rateLimit    = require('express-rate-limit');
const path         = require('path');

const app = express();

// ── MIDDLEWARE ────────────────────────────────────────────────
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Session
app.use(session({
  secret:            'doubtit-secret-2026-' + Math.random(),
  resave:            false,
  saveUninitialized: false,
  cookie: { maxAge: 2 * 60 * 60 * 1000 }, // 2 hours
}));

// Rate limiting for public write endpoints
const submitLimiter = rateLimit({
  windowMs: 60 * 60 * 1000,  // 1 hour
  max:      10,
  message:  { error: 'Too many submissions from this IP. Please try again later.' },
  standardHeaders: true,
  legacyHeaders:   false,
});

// ── STATIC FILES ──────────────────────────────────────────────
app.use(express.static(path.join(__dirname, '../public')));

// ── PUBLIC API STATS ──────────────────────────────────────────
const db = require('./db');

app.get('/api/stats', (req, res) => {
  const stats      = db.getStats();
  const { calculateAllRiskScores } = require('./risk');
  const riskScores = calculateAllRiskScores();
  stats.highRisk   = riskScores.filter(r => r.riskLevel === 'HIGH').length;
  stats.mediumRisk = riskScores.filter(r => r.riskLevel === 'MEDIUM').length;
  stats.lowRisk    = riskScores.filter(r => r.riskLevel === 'LOW').length;
  res.json(stats);
});

// ── ROUTES ────────────────────────────────────────────────────
app.use('/api/campaigns',   require('./routes/campaigns'));
app.use('/api/search',      require('./routes/search'));
app.use('/api/complaints',  require('./routes/complaints'));
app.use('/api/submissions', submitLimiter, require('./routes/submissions'));
app.use('/api/admin',       require('./routes/admin'));

// ── RISK REPORT DOWNLOAD ──────────────────────────────────────
app.get('/api/risk-report', (req, res) => {
  const { calculateAllRiskScores } = require('./risk');
  const scores = calculateAllRiskScores().sort((a, b) => b.score - a.score);
  const lines  = [
    'DOUBTIT – RISK REPORT',
    `Generated: ${new Date().toLocaleString('en-IN')}`,
    `Total campaigns: ${scores.length}`,
    '='.repeat(70),
    '',
    ...scores.map((r, i) => [
      `Rank ${i + 1}: ${r.campaign.campaign_name} (${r.campaign.campaign_id})`,
      `  Organisation : ${r.campaign.organization_name}`,
      `  Risk Level   : ${r.riskLevel}  |  Score: ${r.score}/100`,
      `  Complaints   : ${r.complaintCount}`,
      `  Factors      : ${r.reasons.length ? r.reasons.join('; ') : 'None'}`,
      '',
    ].join('\n')),
    '='.repeat(70),
    'DISCLAIMER: This report contains synthetic test data for educational purposes only.',
  ];
  res.setHeader('Content-Type', 'text/plain; charset=utf-8');
  res.setHeader('Content-Disposition', 'attachment; filename="doubtit-risk-report.txt"');
  res.send(lines.join('\n'));
});

// ── SPA FALLBACK: serve index for unknown routes ──────────────
app.get('*', (req, res) => {
  // Don't intercept API calls
  if (req.path.startsWith('/api/')) {
    return res.status(404).json({ error: 'API endpoint not found.' });
  }
  res.sendFile(path.join(__dirname, '../public/index.html'));
});

// ── START ─────────────────────────────────────────────────────
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`\n========== DOUBTIT WEB SERVER ==========`);
  console.log(`Server running at  http://localhost:${PORT}`);
  console.log(`Admin login        http://localhost:${PORT}/admin/login.html`);
  console.log(`Admin credentials  username: admin  |  password: admin123`);
  console.log(`=========================================\n`);
});
