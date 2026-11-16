/* search.js – Search page */

(function () {
  const input    = document.getElementById('search-input');
  const stateEl  = document.getElementById('search-state');
  const resultsEl = document.getElementById('search-results');
  let   activeType = 'all';
  let   searchTimer;

  // Pre-fill from URL param
  const preQ = getParam('q');
  if (preQ) { input.value = preQ; runSearch(preQ, activeType); }

  // Type buttons
  document.querySelectorAll('.search-type-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.search-type-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      activeType = btn.dataset.type;
      if (input.value.trim().length >= 2) runSearch(input.value.trim(), activeType);
    });
  });

  // Input
  input.addEventListener('input', () => {
    clearTimeout(searchTimer);
    const q = input.value.trim();
    if (q.length < 2) { stateEl.innerHTML = ''; resultsEl.innerHTML = ''; return; }
    searchTimer = setTimeout(() => runSearch(q, activeType), 320);
  });
  input.addEventListener('keydown', e => {
    if (e.key === 'Enter') { clearTimeout(searchTimer); runSearch(input.value.trim(), activeType); }
  });

  async function runSearch(q, type) {
    if (q.length < 2) return;
    stateEl.innerHTML = `<div class="loading-row"><div class="spinner"></div> Searching…</div>`;
    resultsEl.innerHTML = '';
    try {
      const data = await API.get(`/api/search?q=${encodeURIComponent(q)}&type=${encodeURIComponent(type)}`);
      stateEl.innerHTML = `<p class="text-sm text-muted mb-16">${data.total} result${data.total !== 1 ? 's' : ''} for "<strong>${escHtml(q)}</strong>"</p>`;
      if (!data.results.length) {
        resultsEl.innerHTML = `
          <div class="empty-state">
            <svg width="40" height="40" viewBox="0 0 40 40" fill="none"><circle cx="18" cy="18" r="12" stroke="currentColor" stroke-width="2"/><path d="M28 28l8 8" stroke="currentColor" stroke-width="2" stroke-linecap="round"/></svg>
            <h3>Campaign not found</h3>
            <p>No campaign matches "<strong>${escHtml(q)}</strong>" in our database.</p>
            <a href="/submit.html?name=${encodeURIComponent(q)}" class="btn btn-primary">Submit this campaign for review →</a>
          </div>`;
        return;
      }
      resultsEl.innerHTML = `
        <div class="table-wrap">
          <table class="table">
            <thead><tr><th>Campaign</th><th>Organisation</th><th>Review Status</th><th>Risk Level</th><th>Score</th><th></th></tr></thead>
            <tbody>
              ${data.results.map(r => `
                <tr>
                  <td class="name-cell">
                    <a href="/campaign.html?id=${escHtml(r.campaign.campaign_id)}">${escHtml(r.campaign.campaign_name)}</a><br>
                    <span class="text-xs text-mono text-muted">${escHtml(r.campaign.campaign_id)}</span>
                  </td>
                  <td class="text-sm text-muted">${escHtml(r.campaign.organization_name)}</td>
                  <td>${statusBadge(r.campaign.review_status)}</td>
                  <td>${riskBadge(r.risk.riskLevel)}</td>
                  <td>${riskScoreBar(r.risk.score, r.risk.riskLevel)}</td>
                  <td><a href="/campaign.html?id=${escHtml(r.campaign.campaign_id)}" class="btn btn-sm btn-secondary">View</a></td>
                </tr>`).join('')}
            </tbody>
          </table>
        </div>`;
    } catch (e) {
      stateEl.innerHTML = '';
      showAlert(resultsEl, 'error', e.message || 'Search failed. Try again.');
    }
  }
})();
