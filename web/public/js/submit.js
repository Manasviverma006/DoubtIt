/* submit.js – Submit Campaign form */

(function () {
  const form        = document.getElementById('submit-form');
  const submitBtn   = document.getElementById('submit-btn');
  const alertBox    = document.getElementById('form-alert');
  const formCont    = document.getElementById('form-container');
  const successCont = document.getElementById('success-container');
  const descCount   = document.getElementById('desc-count');
  const reasonCount = document.getElementById('reason-count');

  // Live character counters
  document.getElementById('campaign_description').addEventListener('input', function () {
    descCount.textContent = this.value.length;
  });
  document.getElementById('reason_for_reporting').addEventListener('input', function () {
    reasonCount.textContent = this.value.length;
  });

  // Pre-fill name from URL
  const preName = new URLSearchParams(window.location.search).get('name');
  if (preName) document.getElementById('campaign_name').value = preName;

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
      campaign_name:        form.campaign_name.value.trim(),
      campaign_url:         form.campaign_url.value.trim(),
      organization_name:    form.organization_name.value.trim(),
      campaign_description: form.campaign_description.value.trim(),
      campaign_category:    form.campaign_category.value.trim(),
      reason_for_reporting: form.reason_for_reporting.value.trim(),
      evidence_links:       form.evidence_links.value.trim(),
      submitter_contact:    form.submitter_contact.value.trim(),
    };

    // Client-side validation
    let valid = true;
    if (!data.campaign_name || data.campaign_name.length < 3) {
      fieldError('campaign_name', 'Campaign name must be at least 3 characters.'); valid = false;
    }
    if (!data.organization_name || data.organization_name.length < 2) {
      fieldError('organization_name', 'Organisation name is required.'); valid = false;
    }
    if (!data.campaign_description || data.campaign_description.length < 50) {
      fieldError('campaign_description', 'Description must be at least 50 characters.'); valid = false;
    }
    if (!data.reason_for_reporting || data.reason_for_reporting.length < 30) {
      fieldError('reason_for_reporting', 'Reason must be at least 30 characters.'); valid = false;
    }
    if (data.campaign_url) {
      try { new URL(data.campaign_url); }
      catch { fieldError('campaign_url', 'Enter a valid URL starting with http:// or https://'); valid = false; }
    }
    if (data.submitter_contact && !/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(data.submitter_contact)) {
      fieldError('submitter_contact', 'Enter a valid email address.'); valid = false;
    }

    if (!valid) return;

    submitBtn.disabled = true;
    submitBtn.innerHTML = `<div class="spinner sm"></div> Submitting…`;

    try {
      const res = await API.post('/api/submissions', data);
      formCont.classList.add('hidden');
      successCont.classList.remove('hidden');
      document.getElementById('conf-id').textContent = res.submission_id;
    } catch (err) {
      showAlert(alertBox, 'error', err.message || 'Submission failed. Please try again.');
      submitBtn.disabled = false;
      submitBtn.innerHTML = `<svg width="16" height="16" viewBox="0 0 16 16" fill="none"><path d="M14 2L7 9M14 2H9M14 2V7" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round"/></svg> Submit for Review`;
    }
  });
})();
