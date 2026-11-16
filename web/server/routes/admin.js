const express = require('express');
const router  = express.Router();
const db      = require('../db');
const { requireAdmin } = require('../middleware/auth');

// POST /api/admin/login
router.post('/login', (req, res) => {
  const { username, password } = req.body;
  if (!username || !password)
    return res.status(400).json({ error: 'Username and password are required.' });

  if (!db.verifyAdmin(username.trim(), password)) {
    return res.status(401).json({ error: 'Invalid credentials.' });
  }

  req.session.isAdmin       = true;
  req.session.adminUsername = username.trim();
  res.json({ success: true, username: username.trim() });
});

// POST /api/admin/logout
router.post('/logout', (req, res) => {
  req.session.destroy();
  res.json({ success: true });
});

// GET /api/admin/me
router.get('/me', requireAdmin, (req, res) => {
  res.json({ isAdmin: true, username: req.session.adminUsername });
});

// GET /api/admin/stats
router.get('/stats', requireAdmin, (req, res) => {
  const stats = db.getStats();
  res.json(stats);
});

module.exports = router;
