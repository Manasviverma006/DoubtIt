/* admin/review.js */

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

  document.getElementById('logout-btn').addEventListener('click', async () => {
    await API.post('/api/admin/logout', {});
    window.location = '/admin/login.html';
  });

  const subId   = getParam('id');
  const content = document.getElementById('review-content');

  if (!subId) {
    content.innerHTML = `<div class="alert alert-error"><div>No submission ID specified. <a href="/admin/dashboard.html">Back to dashboard</a>.</div></div>`;
    return;
  }

  let submissionData;

  async function loadSubmission() {
    content.innerHTML = `<div class="loading-row"><div class="spinner"></div> Loading…</div>`;
    try {
      submissionData = await API.get(`/api/submissions/${encodeURIComponent(subId)}`);
      render(submissionData);
    } catch (e) {
      content.innerHTML = `<div class="alert alert-error"><div>${escHtml(e.message || 'Submission not found.')}</div></div>`;
    }
  }

  function render({ submission: s, reviews }) {
    document.getElementById('review-title').textContent    = s.campaign_name;
    document.getElementById('review-subtitle').textContent = `${s.submission_id} · Submitted ${formatDate(s.submission_date)}`;

    const statusCls = {
      'Pending Review': 'pending', 'Verified Legitimate': 'verified',
      'Fraudulent': 'fraudulent', 'Suspicious': 'suspicious',
      'Rejected': 'rejected', 'Needs More Information': 'needs',
    };

    content.innerHTML = `
      <!-- HEADER ACTIONS -->
      <div style="display:flex;align-items:center;justify-content:space-between;gap:16px;margin-bottom:24px;flex-wrap:wrap">
        <div style="display:flex;align-items:center;gap:12px">
          <span>Current status:</span> ${statusBadge(s.review_status)}
        </div>
        ${s.review_status === 'Pending Review' || s.review_status === 'Needs More Information'
          ? `<button class="btn btn-primary" id="open-review-btn">Record Decision</button>`
          : `<button class="btn btn-secondary" id="open-review-btn">Update Decision</button>`}
      </div>

      <!-- SUBMISSION DETAILS -->
      <div class="row mb-24">
        <div class="col">
          <div class="card">
            <div class="card-header"><span class="card-title">Campaign Details</span></div>
            <div class="card-body">
              <div class="info-grid">
                <span class="info-label">Campaign Name</span>  <span class="info-value">${escHtml(s.campaign_name)}</span>
                <span class="info-label">Organisation</span>   <span class="info-value">${escHtml(s.organization_name)}</span>
                <span class="info-label">Category</span>       <span class="info-value">${escHtml(s.campaign_category || '—')}</span>
                <span class="info-label">Website / URL</span>  <span class="info-value">
                  ${s.campaign_url ? `<a href="${escHtml(s.campaign_url)}" target="_blank" rel="noopener">${escHtml(s.campaign_url)}</a>` : '—'}
                </span>
                <span class="info-label">Submitted IP</span>   <span class="info-value mono">${escHtml(s.submitted_ip || '—')}</span>
                <span class="info-label">Submitter Email</span><span class="info-value">${escHtml(s.submitter_contact || '—')}</span>
              </div>
            </div>
          </div>
        </div>
      </div>

      <!-- DESCRIPTION & REASON -->
      <div class="card mb-24">
        <div class="card-header"><span class="card-title">Campaign Description</span></div>
        <div class="card-body"><p style="line-height:1.75;white-space:pre-wrap">${escHtml(s.campaign_description)}</p></div>
      </div>

      <div class="card mb-24">
        <div class="card-header"><span class="card-title">Reason for Reporting</span></div>
        <div class="card-body"><p style="line-height:1.75;white-space:pre-wrap">${escHtml(s.reason_for_reporting)}</p></div>
      </div>

      ${s.evidence_links ? `
      <div class="card mb-24">
        <div class="card-header"><span class="card-title">Evidence Links</span></div>
        <div class="card-body">
          ${s.evidence_links.split('\n').filter(Boolean).map(link =>
            `<div class="mb-8"><a href="${escHtml(link.trim())}" target="_blank" rel="noopener" class="text-sm">${escHtml(link.trim())}</a></div>`
          ).join('')}
        </div>
      </div>` : ''}

      <!-- REVIEW HISTORY -->
      <div class="card mb-24">
        <div class="card-header"><span class="card-title">Review History</span><span class="text-sm text-muted">${reviews.length} decision${reviews.length !== 1 ? 's' : ''}</span></div>
        <div class="card-body">
          ${reviews.length === 0
            ? `<p class="text-sm text-muted">No review decisions recorded yet.</p>`
            : `<div class="timeline">
                ${reviews.map(r => `
                  <div class="timeline-item">
                    <div class="timeline-date">${formatDate(r.review_date)} · ${escHtml(r.reviewer_username)}</div>
                    <div class="timeline-title">${statusBadge(r.review_decision)}</div>
                    ${r.review_notes ? `<div class="timeline-body mt-4">${escHtml(r.review_notes)}</div>` : ''}
                    <div class="text-xs text-muted mt-4">${escHtml(r.review_id)}</div>
                  </div>`).join('')}
              </div>`}
        </div>
      </div>`;

    // Wire up review modal
    document.getElementById('open-review-btn').addEventListener('click', openModal);
  }

  // ── MODAL ──────────────────────────────────────────────────
  const modal      = document.getElementById('review-modal');
  const modalClose = document.getElementById('modal-close');
  const modalCancel= document.getElementById('modal-cancel');
  const modalSubmit= document.getElementById('modal-submit');
  const modalAlert = document.getElementById('modal-alert');

  function openModal()  { modal.classList.add('open'); }
  function closeModal() { modal.classList.remove('open'); modalAlert.innerHTML = ''; }

  modalClose.addEventListener('click',  closeModal);
  modalCancel.addEventListener('click', closeModal);
  modal.addEventListener('click', e => { if (e.target === modal) closeModal(); });

  modalSubmit.addEventListener('click', async () => {
    modalAlert.innerHTML = '';
    const decision = document.getElementById('review_decision').value;
    const notes    = document.getElementById('review_notes').value.trim();

    if (!decision) {
      showAlert(modalAlert, 'error', 'Please select a decision.');
      return;
    }

    modalSubmit.disabled = true;
    modalSubmit.innerHTML = `<div class="spinner sm"></div> Saving…`;

    try {
      await API.post(`/api/submissions/${encodeURIComponent(subId)}/review`, {
        review_decision: decision,
        review_notes:    notes,
      });
      closeModal();
      loadSubmission(); // Refresh
    } catch (e) {
      showAlert(modalAlert, 'error', e.message || 'Failed to save decision.');
      modalSubmit.disabled = false;
      modalSubmit.textContent = 'Submit Decision';
    }
  });

  loadSubmission();
})();
