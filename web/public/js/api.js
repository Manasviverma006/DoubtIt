/* api.js – Shared fetch wrapper */

const API = {
  async get(url) {
    const r = await fetch(url);
    const data = await r.json();
    if (!r.ok) throw new Error(data.error || 'Request failed');
    return data;
  },

  async post(url, body) {
    const r = await fetch(url, {
      method:  'POST',
      headers: { 'Content-Type': 'application/json' },
      body:    JSON.stringify(body),
    });
    const data = await r.json();
    if (!r.ok) throw new Error(data.error || (data.errors && data.errors.join(' | ')) || 'Request failed');
    return data;
  },
};

/* Helpers */
function riskBadge(level) {
  const map = { HIGH:'high', MEDIUM:'medium', LOW:'low' };
  const cls = map[level] || 'low';
  return `<span class="badge badge-${cls}"><span class="badge-dot"></span>${level}</span>`;
}

function riskScoreBar(score, level) {
  const cls = level === 'HIGH' ? 'high' : level === 'MEDIUM' ? 'medium' : 'low';
  return `<div class="score-bar-wrap">
    <div class="score-bar"><div class="score-bar-fill ${cls}" style="width:${score}%"></div></div>
    <span class="score-num ${cls === 'high' ? 'stat-high' : cls === 'medium' ? 'stat-medium' : 'stat-low'}">${score}</span>
  </div>`;
}

function statusBadge(status) {
  const map = {
    'Under Review': 'pending', 'Awaiting documents': 'needs', 'Pending': 'pending',
    'Verified Legitimate': 'verified', 'Fraudulent': 'fraudulent',
    'Suspicious': 'suspicious', 'Needs More Information': 'needs',
    'Rejected': 'rejected', 'Pending Review': 'pending',
  };
  const cls = map[status] || 'pending';
  return `<span class="badge badge-${cls}">${status}</span>`;
}

function escHtml(str) {
  return String(str || '').replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;');
}

function formatDate(str) {
  if (!str) return '—';
  const d = new Date(str);
  return isNaN(d) ? str : d.toLocaleDateString('en-IN', { day:'2-digit', month:'short', year:'numeric' });
}

function getParam(name) {
  return new URLSearchParams(window.location.search).get(name) || '';
}

function showAlert(container, type, message) {
  const icons = {
    success: '<svg width="16" height="16" viewBox="0 0 16 16" fill="none"><path d="M13.5 4L6 11.5L2.5 8" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round"/></svg>',
    error:   '<svg width="16" height="16" viewBox="0 0 16 16" fill="none"><circle cx="8" cy="8" r="6.5" stroke="currentColor" stroke-width="1.5"/><path d="M8 5v3.5M8 11h.01" stroke="currentColor" stroke-width="1.75" stroke-linecap="round"/></svg>',
    info:    '<svg width="16" height="16" viewBox="0 0 16 16" fill="none"><circle cx="8" cy="8" r="6.5" stroke="currentColor" stroke-width="1.5"/><path d="M8 7.5v4M8 5h.01" stroke="currentColor" stroke-width="1.75" stroke-linecap="round"/></svg>',
    warning: '<svg width="16" height="16" viewBox="0 0 16 16" fill="none"><path d="M8 2L14.5 13.5H1.5L8 2Z" stroke="currentColor" stroke-width="1.5" stroke-linejoin="round"/><path d="M8 7v3M8 11.5h.01" stroke="currentColor" stroke-width="1.75" stroke-linecap="round"/></svg>',
  };
  container.innerHTML = `<div class="alert alert-${type}">${icons[type]||''}<div>${escHtml(message)}</div></div>`;
  container.scrollIntoView({ behavior:'smooth', block:'nearest' });
}
