/* campaigns.js – Browse campaigns page */

(function () {
  let currentRisk = 'all';
  let currentSort = 'score';
  let currentPage = 1;
  const LIMIT = 10;

  const container  = document.getElementById('campaigns-container');
  const pagination = document.getElementById('pagination');

  async function load() {
    container.innerHTML = `<div class="loading-row"><div class="spinner"></div> Loading…</div>`;
    pagination.innerHTML = '';
    try {
      const qs   = `risk=${currentRisk === 'all' ? '' : currentRisk}&sort=${currentSort}&page=${currentPage}&limit=${LIMIT}`;
      const data = await API.get(`/api/campaigns?${qs}`);
      renderTable(data);
      renderPagination(data.total, data.page, data.limit);
    } catch (e) {
      container.innerHTML = `<div class="alert alert-error"><div>${escHtml(e.message)}</div></div>`;
    }
  }

  function renderTable(data) {
    if (!data.campaigns.length) {
      container.innerHTML = `<div class="empty-state"><h3>No campaigns found</h3><p>Try a different filter.</p></div>`;
      return;
    }
    const start = (data.page - 1) * data.limit + 1;
    container.innerHTML = `
      <p class="text-sm text-muted mb-12">Showing ${start}–${Math.min(start + data.campaigns.length - 1, data.total)} of ${data.total} campaigns</p>
      <div class="table-wrap">
        <table class="table">
          <thead><tr>
            <th>ID</th><th>Campaign</th><th>Organisation</th>
            <th>Review Status</th><th>Risk Level</th><th>Score</th><th>Complaints</th><th></th>
          </tr></thead>
          <tbody>
            ${data.campaigns.map(r => `
              <tr>
                <td class="id-cell">${escHtml(r.campaign.campaign_id)}</td>
                <td class="name-cell">
                  <a href="/campaign.html?id=${escHtml(r.campaign.campaign_id)}">${escHtml(r.campaign.campaign_name)}</a>
                </td>
                <td class="text-sm text-muted">${escHtml(r.campaign.organization_name)}</td>
                <td>${statusBadge(r.campaign.review_status)}</td>
                <td>${riskBadge(r.riskLevel)}</td>
                <td>${riskScoreBar(r.score, r.riskLevel)}</td>
                <td class="text-sm">${r.complaintCount}</td>
                <td><a href="/campaign.html?id=${escHtml(r.campaign.campaign_id)}" class="btn btn-sm btn-secondary">View</a></td>
              </tr>`).join('')}
          </tbody>
        </table>
      </div>`;
  }

  function renderPagination(total, page, limit) {
    const totalPages = Math.ceil(total / limit);
    if (totalPages <= 1) return;
    let html = `<button class="page-btn" ${page <= 1 ? 'disabled' : ''} data-p="${page - 1}">‹</button>`;
    for (let i = 1; i <= totalPages; i++) {
      if (i === 1 || i === totalPages || Math.abs(i - page) <= 2) {
        html += `<button class="page-btn ${i === page ? 'active' : ''}" data-p="${i}">${i}</button>`;
      } else if (Math.abs(i - page) === 3) {
        html += `<span style="padding:0 4px;color:var(--text-muted)">…</span>`;
      }
    }
    html += `<button class="page-btn" ${page >= totalPages ? 'disabled' : ''} data-p="${page + 1}">›</button>`;
    pagination.innerHTML = html;
    pagination.querySelectorAll('.page-btn:not([disabled])').forEach(btn => {
      btn.addEventListener('click', () => { currentPage = parseInt(btn.dataset.p); load(); });
    });
  }

  // Filters
  document.querySelectorAll('.filter-btn[data-risk]').forEach(btn => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.filter-btn[data-risk]').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      currentRisk = btn.dataset.risk;
      currentPage = 1;
      load();
    });
  });

  document.getElementById('sort-select').addEventListener('change', e => {
    currentSort = e.target.value;
    currentPage = 1;
    load();
  });

  load();
})();
