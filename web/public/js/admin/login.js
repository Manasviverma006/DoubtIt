/* admin/login.js */

(async function () {
  // If already logged in, go straight to dashboard
  try {
    await API.get('/api/admin/me');
    window.location = '/admin/dashboard.html';
    return;
  } catch {}

  const form     = document.getElementById('login-form');
  const alertBox = document.getElementById('login-alert');
  const btn      = document.getElementById('login-btn');

  form.addEventListener('submit', async (e) => {
    e.preventDefault();
    alertBox.innerHTML = '';
    btn.disabled = true;
    btn.innerHTML = `<div class="spinner sm"></div> Signing in…`;

    try {
      await API.post('/api/admin/login', {
        username: form.username.value.trim(),
        password: form.password.value,
      });
      window.location = '/admin/dashboard.html';
    } catch (err) {
      showAlert(alertBox, 'error', err.message || 'Invalid credentials.');
      btn.disabled = false;
      btn.textContent = 'Sign In';
    }
  });
})();
