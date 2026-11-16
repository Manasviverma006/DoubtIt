/* nav.js – Navigation active link highlighting + mobile toggle */

(function () {
  const path = window.location.pathname;
  document.querySelectorAll('.nav-link').forEach(link => {
    const href = link.getAttribute('href');
    if (!href) return;
    if (href === '/' || href === '/index.html') {
      if (path === '/' || path === '/index.html') link.classList.add('active');
    } else if (path.includes(href.replace('.html', ''))) {
      link.classList.add('active');
    }
  });

  const mobileBtn = document.getElementById('nav-mobile-btn');
  const navLinks  = document.getElementById('nav-links');
  if (mobileBtn && navLinks) {
    mobileBtn.addEventListener('click', () => navLinks.classList.toggle('open'));
    document.addEventListener('click', e => {
      if (!mobileBtn.contains(e.target) && !navLinks.contains(e.target)) {
        navLinks.classList.remove('open');
      }
    });
  }
})();
