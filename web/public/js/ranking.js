/* ranking.js – Risk Ranking page */

(async function () {
  const container = document.getElementById('ranking-container');
  try {
    const data = await API.get('/api/campaigns/ranking');
    if (!data.length) {
      container.innerHTML = `<div class="empty-state"><h3>No data</h3></div>`;
      return;
    }
    container.innerHTML = `
      <div class="table-wrap">
        <table class="table">
          <thead><tr>
            <th style="width:52px">Rank</th>
            <th>Campaign</th>
            <th>Organisation</th>
            <th>Risk Level</th>
            <th style="min-width:160px">Score</th>
            <th>Complaints</th>
            <th>Risk Factors</th>
            <th></th>
          </tr></thead>
          <tbody>
            ${data.map((r, i) => {
              const rank = i + 1;
              const rankHtml = rank <= 3
                ? `<span class="rank-badge rank-${rank}">${rank}</span>`
                : `<span class="text-sm text-muted">${rank}</span>`;
              return `<tr>
                <td style="text-align:center">${rankHtml}</td>
                <td class="name-cell">
                  <a href="/campaign.html?id=${escHtml(r.campaign.campaign_id)}">${escHtml(r.campaign.campaign_name)}</a><br>
                  <span class="text-xs text-mono text-muted">${escHtml(r.campaign.campaign_id)}</span>
                </td>
                <td class="text-sm text-muted">${escHtml(r.campaign.organization_name)}</td>
                <td>${riskBadge(r.riskLevel)}</td>
                <td>${riskScoreBar(r.score, r.riskLevel)}</td>
                <td class="text-sm">${r.complaintCount}</td>
                <td class="text-sm text-muted">${r.reasons.length > 0 ? escHtml(r.reasons[0]) + (r.reasons.length > 1 ? ` +${r.reasons.length-1} more` : '') : '—'}</td>
                <td><a href="/campaign.html?id=${escHtml(r.campaign.campaign_id)}" class="btn btn-sm btn-secondary">View</a></td>
              </tr>`;
            }).join('')}
          </tbody>
        </table>
      </div>`;
  } catch (e) {
    container.innerHTML = `<div class="alert alert-error"><div>${escHtml(e.message)}</div></div>`;
  }
})();
