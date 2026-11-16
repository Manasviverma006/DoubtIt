/* campaign.js – Campaign detail page */

(async function () {
  const id = getParam('id');
  if (!id) {
    document.getElementById('page-title').textContent = 'Campaign Not Found';
    document.getElementById('main-content').innerHTML =
      `<div class="alert alert-error"><div>No campaign ID specified. <a href="/campaigns.html">Browse all campaigns</a>.</div></div>`;
    return;
  }

  try {
    const d = await API.get(`/api/campaigns/${encodeURIComponent(id)}`);
    const { campaign, risk, complaints, evidence, connections } = d;

    // Page header
    document.title = `${campaign.campaign_name} – DoubtIt`;
    document.getElementById('page-title').textContent    = campaign.campaign_name;
    document.getElementById('page-subtitle').textContent = campaign.organization_name + ' · ' + campaign.campaign_id;
    document.getElementById('page-badges').innerHTML     = riskBadge(risk.riskLevel);

    const riskClass = risk.riskLevel === 'HIGH' ? 'high' : risk.riskLevel === 'MEDIUM' ? 'medium' : 'low';

    document.getElementById('main-content').innerHTML = `

      <!-- TOP ROW: Risk + Info -->
      <div class="row mb-24">
        <div class="col" style="max-width:220px;flex:0 0 auto">
          <div class="card">
            <div class="card-body risk-meter">
              <div class="risk-score-big ${riskClass}">${risk.score}</div>
              <div class="risk-score-label">Risk Score (out of 100)</div>
              <div style="margin:12px 0">${riskBadge(risk.riskLevel)}</div>
              <div class="score-bar" style="margin-top:8px">
                <div class="score-bar-fill ${riskClass}" style="width:${risk.score}%"></div>
              </div>
            </div>
          </div>
        </div>

        <div class="col">
          <div class="card">
            <div class="card-header"><span class="card-title">Campaign Information</span></div>
            <div class="card-body">
              <div class="info-grid">
                <span class="info-label">Campaign ID</span>    <span class="info-value mono">${escHtml(campaign.campaign_id)}</span>
                <span class="info-label">Organisation</span>   <span class="info-value">${escHtml(campaign.organization_name)}</span>
                <span class="info-label">Website</span>        <span class="info-value">${campaign.website ? `<a href="${escHtml(campaign.website)}" rel="noopener noreferrer" target="_blank">${escHtml(campaign.website)}</a>` : '—'}</span>
                <span class="info-label">UPI ID</span>         <span class="info-value mono">${escHtml(campaign.upi_id || '—')}</span>
                <span class="info-label">Bank Account</span>   <span class="info-value mono">${escHtml(campaign.bank_account_id || '—')}</span>
                <span class="info-label">Phone ID</span>       <span class="info-value mono">${escHtml(campaign.phone_id || '—')}</span>
                <span class="info-label">Review Status</span>  <span class="info-value">${statusBadge(campaign.review_status)}</span>
                <span class="info-label">Source</span>         <span class="info-value">${escHtml(campaign.source || 'Dataset')}</span>
              </div>
            </div>
          </div>
        </div>
      </div>

      <!-- RISK FACTORS -->
      <div class="card section mb-24">
        <div class="card-header">
          <span class="card-title">Risk Factors Detected</span>
          <span class="text-sm text-muted">${risk.reasons.length} factor${risk.reasons.length !== 1 ? 's' : ''}</span>
        </div>
        <div class="card-body">
          ${risk.reasons.length === 0
            ? `<p class="text-muted text-sm">No risk factors detected. This does not guarantee legitimacy.</p>`
            : `<ul style="display:flex;flex-direction:column;gap:10px">
                ${risk.reasons.map(r => `
                  <li style="display:flex;align-items:flex-start;gap:10px">
                    <svg width="16" height="16" viewBox="0 0 16 16" fill="none" style="flex-shrink:0;margin-top:2px"><path d="M8 2L14 13H2L8 2Z" stroke="var(--risk-high)" stroke-width="1.5" stroke-linejoin="round"/><path d="M8 7v2.5M8 11h.01" stroke="var(--risk-high)" stroke-width="1.5" stroke-linecap="round"/></svg>
                    <span style="font-size:0.9rem">${escHtml(r)}</span>
                  </li>`).join('')}
              </ul>`}
        </div>
      </div>

      <!-- CONNECTIONS / GRAPH -->
      ${connections.length > 0 ? `
      <div class="card section mb-24">
        <div class="card-header">
          <span class="card-title">Connected Campaigns</span>
          <span class="badge badge-high">${connections.length} connection${connections.length !== 1 ? 's' : ''}</span>
        </div>
        <div class="card-body">
          <div class="table-wrap">
            <table class="table">
              <thead><tr><th>Related Campaign</th><th>Relationship Type</th><th>Shared Identifier</th><th>Interpretation</th><th></th></tr></thead>
              <tbody>
                ${connections.map(c => `
                  <tr>
                    <td class="name-cell"><a href="/campaign.html?id=${escHtml(c.other_campaign_id)}">${escHtml(c.other_campaign_name)}</a></td>
                    <td><span class="badge badge-suspicious">${escHtml(c.relationship_type)}</span></td>
                    <td class="text-mono text-sm">${escHtml(c.shared_identifier || '—')}</td>
                    <td class="text-sm text-muted">${escHtml(c.interpretation || '—')}</td>
                    <td><a href="/campaign.html?id=${escHtml(c.other_campaign_id)}" class="btn btn-sm btn-ghost">View →</a></td>
                  </tr>`).join('')}
              </tbody>
            </table>
          </div>
        </div>
      </div>` : ''}

      <!-- COMPLAINTS -->
      <div class="card section mb-24">
        <div class="card-header">
          <span class="card-title">Complaint History</span>
          <span class="text-sm text-muted">${complaints.length} complaint${complaints.length !== 1 ? 's' : ''}</span>
        </div>
        <div class="card-body">
          ${complaints.length === 0
            ? `<p class="text-sm text-muted">No complaints on record.</p>`
            : `<div class="timeline">
                ${complaints.map(c => `
                  <div class="timeline-item">
                    <div class="timeline-date">${formatDate(c.date_received)} · ${escHtml(c.report_channel || '—')} · <span class="badge badge-pending">${escHtml(c.complaint_category)}</span></div>
                    <div class="timeline-title">${escHtml(c.complaint_description)}</div>
                    <div class="text-xs text-muted mt-4">Status: ${escHtml(c.complaint_status)} · ID: ${escHtml(c.complaint_id)}</div>
                  </div>`).join('')}
              </div>`}
        </div>
        <div class="card-footer">
          <a href="/report.html?campaign_id=${encodeURIComponent(campaign.campaign_id)}" class="btn btn-secondary btn-sm">
            + File a Complaint
          </a>
        </div>
      </div>

      <!-- EVIDENCE -->
      ${evidence.length > 0 ? `
      <div class="card section mb-24">
        <div class="card-header">
          <span class="card-title">Evidence Records</span>
          <span class="text-sm text-muted">${evidence.length} record${evidence.length !== 1 ? 's' : ''}</span>
        </div>
        <div class="card-body">
          <div class="table-wrap">
            <table class="table">
              <thead><tr><th>ID</th><th>Type</th><th>Description</th><th>Source</th><th>Verification</th></tr></thead>
              <tbody>
                ${evidence.map(e => `
                  <tr>
                    <td class="id-cell">${escHtml(e.evidence_id)}</td>
                    <td class="text-sm">${escHtml(e.evidence_type)}</td>
                    <td class="text-sm">${escHtml(e.evidence_description)}</td>
                    <td class="text-sm text-muted">${escHtml(e.source_reference || '—')}</td>
                    <td>${statusBadge(e.verification_status)}</td>
                  </tr>`).join('')}
              </tbody>
            </table>
          </div>
        </div>
      </div>` : ''}

    `;
  } catch (e) {
    document.getElementById('page-title').textContent = 'Campaign Not Found';
    document.getElementById('main-content').innerHTML =
      `<div class="alert alert-error"><div>${escHtml(e.message || 'Campaign not found.')} <a href="/campaigns.html">Browse all campaigns</a>.</div></div>`;
  }
})();
