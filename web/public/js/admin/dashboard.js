/* admin/dashboard.js */

(async function () {
  // Auth guard
  let me;
  try {
    me = await API.get('/api/admin/me');
  } catch {
    window.location = '/admin/login.html';
    return;
  }
  document.getElementById('admin-user-label').textContent = `Logged in as: ${me.username}`;

  // Logout
  document.getElementById('logout-btn').addEventListener('click', async () => {
    await API.post('/api/admin/logout', {});
    window.location = '/admin/login.html';
  });

  // Load stats
  try {
    const stats = await API.get('/api/admin/stats');
    document.getElementById('dash-stats').innerHTML = `
      <div class="stat-card" style="padding:12px 16px">
        <div class="stat-label">Total</div><div class="stat-value" style="font-size:1.5rem">${stats.totalSubmissions}</div>
      </div>
      <div class="stat-card" style="padding:12px 16px">
        <div class="stat-label">Pending</div><div class="stat-value stat-high" style="font-size:1.5rem">${stats.pendingReview}</div>
      </div>`;
  } catch {}

  let currentStatus = 'all';

  // Filter buttons
  document.querySelectorAll('.filter-btn[data-status]').forEach(btn => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.filter-btn[data-status]').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      currentStatus = btn.dataset.status;
      loadSubmissions();
    });
  });

  async function loadSubmissions() {
    const container = document.getElementById('submissions-container');
    container.innerHTML = `<div class="loading-row"><div class="spinner"></div> Loading…</div>`;
    try {
      const qs   = currentStatus !== 'all' ? `?status=${encodeURIComponent(currentStatus)}` : '';
      const data = await API.get(`/api/submissions${qs}`);
      renderSubmissions(data.submissions);
    } catch (e) {
      container.innerHTML = `<div class="alert alert-error"><div>${escHtml(e.message)}</div></div>`;
    }
  }

  function renderSubmissions(list) {
    const container = document.getElementById('submissions-container');
    if (!list.length) {
      container.innerHTML = `
        <div class="empty-state">
          <svg width="40" height="40" viewBox="0 0 40 40" fill="none"><rect x="8" y="6" width="24" height="28" rx="2" stroke="currentColor" stroke-width="2"/><path d="M14 14h12M14 20h12M14 26h8" stroke="currentColor" stroke-width="1.75" stroke-linecap="round"/></svg>
          <h3>No submissions found</h3>
          <p>No submissions match the current filter.</p>
        </div>`;
      return;
    }

    const decisionClass = {
      'Pending Review': 'pending', 'Verified Legitimate': 'verified',
      'Fraudulent': 'fraudulent', 'Suspicious': 'suspicious',
      'Rejected': 'rejected', 'Needs More Information': 'needs',
    };

    container.innerHTML = `
      <div class="table-wrap">
        <table class="table">
          <thead><tr>
            <th>ID</th><th>Campaign</th><th>Organisation</th>
            <th>Category</th><th>Submitted</th><th>Status</th><th></th>
          </tr></thead>
          <tbody>
            ${list.map(s => `
              <tr class="submission-card ${decisionClass[s.review_status] || 'pending'}">
                <td class="id-cell">${escHtml(s.submission_id)}</td>
                <td class="name-cell" style="max-width:220px">
                  <span style="font-weight:600">${escHtml(s.campaign_name)}</span>
                  ${s.campaign_url ? `<br><a href="${escHtml(s.campaign_url)}" target="_blank" rel="noopener" class="text-xs text-muted">${escHtml(s.campaign_url.substring(0,40))}…</a>` : ''}
                </td>
                <td class="text-sm text-muted">${escHtml(s.organization_name)}</td>
                <td class="text-sm">${escHtml(s.campaign_category || '—')}</td>
                <td class="text-sm text-muted">${formatDate(s.submission_date)}</td>
                <td>${statusBadge(s.review_status)}</td>
                <td><a href="/admin/review.html?id=${encodeURIComponent(s.submission_id)}" class="btn btn-sm btn-primary">Review</a></td>
              </tr>`).join('')}
          </tbody>
        </table>
      </div>`;
  }

  loadSubmissions();
})();
