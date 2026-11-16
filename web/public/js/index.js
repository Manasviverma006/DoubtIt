/* index.js – Landing page */

(async function () {
  // Load stats
  try {
    const s = await API.get('/api/stats');
    document.getElementById('stat-campaigns').textContent  = s.totalCampaigns;
    document.getElementById('stat-complaints').textContent = s.totalComplaints;
    document.getElementById('stat-high').textContent       = s.highRisk;
    document.getElementById('stat-medium').textContent     = s.mediumRisk;
    document.getElementById('stat-pending').textContent    = s.pendingReview;
  } catch (e) { console.warn('Stats failed', e); }

  // Load top 5 high-risk campaigns
  try {
    const data = await API.get('/api/campaigns/ranking');
    const top5 = data.slice(0, 5);
    const container = document.getElementById('top-risk-container');

    if (!top5.length) {
      container.innerHTML = `<div class="empty-state"><p>No campaigns found.</p></div>`;
      return;
    }

    container.innerHTML = `
      <div class="table-wrap">
        <table class="table">
          <thead><tr>
            <th>#</th><th>Campaign</th><th>Organisation</th>
            <th>Risk Level</th><th>Score</th><th>Complaints</th>
          </tr></thead>
          <tbody>
            ${top5.map((r, i) => `
              <tr>
                <td class="id-cell">${i + 1}</td>
                <td class="name-cell">
                  <a href="/campaign.html?id=${escHtml(r.campaign.campaign_id)}">
                    ${escHtml(r.campaign.campaign_name)}
                  </a><br>
                  <span class="text-xs text-mono text-muted">${escHtml(r.campaign.campaign_id)}</span>
                </td>
                <td class="text-sm text-muted">${escHtml(r.campaign.organization_name)}</td>
                <td>${riskBadge(r.riskLevel)}</td>
                <td>${riskScoreBar(r.score, r.riskLevel)}</td>
                <td class="text-sm">${r.complaintCount}</td>
              </tr>
            `).join('')}
          </tbody>
        </table>
      </div>`;
  } catch (e) {
    document.getElementById('top-risk-container').innerHTML =
      `<div class="alert alert-error"><div>Failed to load campaigns.</div></div>`;
  }

  // Quick search
  let searchTimer;
  const qi  = document.getElementById('quick-search');
  const qr  = document.getElementById('quick-results');
  qi.addEventListener('input', () => {
    clearTimeout(searchTimer);
    const q = qi.value.trim();
    if (q.length < 2) { qr.innerHTML = ''; return; }
    searchTimer = setTimeout(async () => {
      try {
        const data = await API.get(`/api/search?q=${encodeURIComponent(q)}&type=all`);
        if (!data.results.length) {
          qr.innerHTML = `<p class="text-sm text-muted">No results for "<strong>${escHtml(q)}</strong>". <a href="/submit.html">Submit this campaign →</a></p>`;
          return;
        }
        qr.innerHTML = data.results.slice(0, 4).map(r => `
          <div style="display:flex;align-items:center;justify-content:space-between;padding:10px 0;border-bottom:1px solid var(--border)">
            <div>
              <a href="/campaign.html?id=${escHtml(r.campaign.campaign_id)}" style="font-weight:600;color:var(--text)">
                ${escHtml(r.campaign.campaign_name)}
              </a>
              <span class="text-xs text-muted ml-8">${escHtml(r.campaign.campaign_id)}</span>
            </div>
            <div style="display:flex;align-items:center;gap:10px">
              ${riskBadge(r.risk.riskLevel)}
              <a href="/campaign.html?id=${escHtml(r.campaign.campaign_id)}" class="btn btn-sm btn-ghost">View →</a>
            </div>
          </div>
        `).join('') + (data.total > 4 ? `<p class="text-sm text-muted mt-8"><a href="/search.html?q=${encodeURIComponent(q)}">View all ${data.total} results →</a></p>` : '');
      } catch (e) { qr.innerHTML = ''; }
    }, 300);
  });
  qi.addEventListener('keydown', e => {
    if (e.key === 'Enter') window.location = `/search.html?q=${encodeURIComponent(qi.value.trim())}`;
  });
})();
