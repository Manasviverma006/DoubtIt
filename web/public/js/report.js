/* report.js – File a Complaint */

(function () {
  const form     = document.getElementById('report-form');
  const alertBox = document.getElementById('report-alert');
  const btn      = document.getElementById('report-btn');
  const idDisplay = document.getElementById('report-id-display');

  // Pre-fill campaign_id from URL
  const preId = new URLSearchParams(window.location.search).get('campaign_id');
  if (preId) document.getElementById('campaign_id').value = preId;

  function clearErrors() {
    document.querySelectorAll('.form-error').forEach(el => { el.textContent = ''; el.classList.remove('show'); });
    document.querySelectorAll('.form-control').forEach(el => el.classList.remove('error'));
  }
  function fieldError(id, msg) {
    const inp = document.getElementById(id);
    const err = document.getElementById('err-' + id);
    if (inp) inp.classList.add('error');
    if (err) { err.textContent = msg; err.classList.add('show'); }
  }

  form.addEventListener('submit', async (e) => {
    e.preventDefault();
    clearErrors();
    alertBox.innerHTML = '';

    const data = {
      campaign_id:           form.campaign_id.value.trim(),
      complaint_category:    form.complaint_category.value.trim(),
      complaint_description: form.complaint_description.value.trim(),
      related_identifier:    form.related_identifier.value.trim(),
    };

    let valid = true;
    if (!data.campaign_id) { fieldError('campaign_id', 'Campaign ID is required.'); valid = false; }
    if (!data.complaint_category) { fieldError('complaint_category', 'Please select a category.'); valid = false; }
    if (!data.complaint_description || data.complaint_description.length < 20) {
      fieldError('complaint_description', 'Description must be at least 20 characters.'); valid = false;
    }
    if (!valid) return;

    btn.disabled = true;
    btn.innerHTML = `<div class="spinner sm"></div> Submitting…`;

    try {
      const res = await API.post('/api/complaints', data);
      showAlert(alertBox, 'success', `Complaint filed successfully. Reference ID: ${res.complaint_id}`);
      idDisplay.innerHTML = `<span class="text-mono text-sm">${escHtml(res.complaint_id)}</span>`;
      form.reset();
      btn.textContent = 'Submit Complaint';
    } catch (err) {
      showAlert(alertBox, 'error', err.message || 'Submission failed. Please try again.');
      btn.disabled = false;
      btn.textContent = 'Submit Complaint';
    }
  });
})();
