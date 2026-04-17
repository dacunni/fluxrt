// ---------------------------------------------------------------------------
// FluxRT Dashboard — shared utilities
// ---------------------------------------------------------------------------

// ── Time formatting ─────────────────────────────────────────────────────────

function formatElapsed(seconds) {
  if (seconds == null) return '—';
  if (seconds < 60)   return `${seconds}s`;
  const m = Math.floor(seconds / 60);
  const s = seconds % 60;
  if (m < 60) return `${m}m ${s.toString().padStart(2, '0')}s`;
  const h = Math.floor(m / 60);
  const rm = m % 60;
  return `${h}h ${rm}m`;
}

function formatRelative(isoString) {
  if (!isoString) return '—';
  const diff = Math.floor((Date.now() - new Date(isoString)) / 1000);
  if (diff < 5)   return 'just now';
  if (diff < 60)  return `${diff}s ago`;
  if (diff < 3600) return `${Math.floor(diff / 60)}m ago`;
  if (diff < 86400) return `${Math.floor(diff / 3600)}h ago`;
  return new Date(isoString).toLocaleDateString();
}

function formatDatetime(isoString) {
  if (!isoString) return '—';
  return new Date(isoString).toLocaleString();
}

// ── Status badge HTML ────────────────────────────────────────────────────────

function statusBadge(status) {
  return `<span class="badge badge-${status}">${status}</span>`;
}

// ── Polling ──────────────────────────────────────────────────────────────────

/**
 * Adaptive poller. Calls `fn` immediately, then schedules the next call
 * based on what `fn` returns:
 *   - true  → use fastMs (something is "live")
 *   - false → use slowMs
 * Returns a handle with a `.stop()` method.
 */
function createPoller(fn, fastMs = 2000, slowMs = 10000) {
  let timer = null;
  let stopped = false;

  async function tick() {
    if (stopped) return;
    let live = false;
    try { live = await fn(); } catch (_) {}
    if (!stopped) {
      timer = setTimeout(tick, live ? fastMs : slowMs);
    }
  }

  // Pause/resume on tab visibility
  function onVisibility() {
    if (document.hidden) {
      clearTimeout(timer);
    } else {
      clearTimeout(timer);
      tick();
    }
  }
  document.addEventListener('visibilitychange', onVisibility);

  tick();

  return {
    stop() {
      stopped = true;
      clearTimeout(timer);
      document.removeEventListener('visibilitychange', onVisibility);
    }
  };
}

// ── API helpers ──────────────────────────────────────────────────────────────

async function apiFetch(path, opts = {}) {
  const res = await fetch(path, {
    headers: { 'Content-Type': 'application/json' },
    ...opts,
  });
  if (!res.ok) {
    const body = await res.json().catch(() => ({}));
    throw new Error(body.error || `HTTP ${res.status}`);
  }
  return res.json();
}

async function apiGet(path)       { return apiFetch(path); }
async function apiPost(path, body) { return apiFetch(path, { method: 'POST', body: JSON.stringify(body) }); }

// ── Image grid helpers (shared with job.html) ────────────────────────────────

const LABELS = {
  'trace_color_tone_mapped.png':  'Color (tone mapped)',
  'trace_color.png':              'Color (linear)',
  'trace_isect_distance.png':     'Depth',
  'trace_hit_mask.png':           'Hit mask',
  'trace_isect_normal.png':       'Normal',
  'trace_isect_stddev.png':       'Std dev',
  'trace_isect_mat_diffuse.png':  'Diffuse material',
  'trace_isect_mat_specular.png': 'Specular material',
  'trace_isect_tangent.png':      'Tangent',
  'trace_isect_bitangent.png':    'Bitangent',
  'trace_isect_texcoord.png':     'Texcoord',
};

/**
 * Build the image grid. Mirrors viewer/app.js buildGrid() but accepts
 * an explicit basePath so it works for any job directory.
 */
function buildGridWithPaths(images, basePath, cacheBust = false) {
  const grid = document.getElementById('grid');
  grid.innerHTML = '';
  const ts = Date.now();
  images.forEach(({ file, label }) => {
    const item = document.createElement('div');
    item.className = 'item';

    const h2 = document.createElement('h2');
    h2.textContent = label;

    const img = document.createElement('img');
    img.src = basePath + file + (cacheBust ? `?t=${ts}` : '');
    img.alt = label;
    img.onerror = () => img.classList.add('error');

    item.appendChild(h2);
    item.appendChild(img);
    grid.appendChild(item);
  });
}

/**
 * Refresh images already in the grid by appending a new cache-buster.
 */
function refreshGridImages() {
  const ts = Date.now();
  document.querySelectorAll('.item img').forEach(img => {
    const src = img.src.replace(/\?t=\d+$/, '');
    img.src = `${src}?t=${ts}`;
  });
}

// ── Lens + Lightbox (mirrors viewer/app.js) ───────────────────────────────────

const ZOOM      = 4;
const LENS_SIZE = 200;

function setupLens() {
  const lens   = document.getElementById('lens');
  const canvas = document.getElementById('lens-canvas');
  if (!lens || !canvas) return;
  const ctx = canvas.getContext('2d');
  canvas.width  = LENS_SIZE;
  canvas.height = LENS_SIZE;

  let activeImg = null;

  document.getElementById('grid').addEventListener('mouseover', e => {
    const img = e.target.closest('.item img');
    if (img) { activeImg = img; lens.style.display = 'block'; }
  });
  document.getElementById('grid').addEventListener('mouseout', e => {
    if (!e.target.closest('.item img')) return;
    activeImg = null;
    lens.style.display = 'none';
  });
  document.getElementById('grid').addEventListener('mousemove', e => {
    const img = e.target.closest('.item img');
    if (!img || !activeImg) return;
    const rect    = img.getBoundingClientRect();
    const scaleX  = img.naturalWidth  / rect.width;
    const scaleY  = img.naturalHeight / rect.height;
    const px      = (e.clientX - rect.left) * scaleX;
    const py      = (e.clientY - rect.top)  * scaleY;
    const srcSize = LENS_SIZE / ZOOM;
    const sx      = Math.max(0, px - srcSize / 2);
    const sy      = Math.max(0, py - srcSize / 2);
    ctx.clearRect(0, 0, LENS_SIZE, LENS_SIZE);
    ctx.imageSmoothingEnabled = false;
    try { ctx.drawImage(img, sx, sy, srcSize, srcSize, 0, 0, LENS_SIZE, LENS_SIZE); }
    catch (_) {}
    const offset = 16;
    let lx = e.clientX + offset, ly = e.clientY + offset;
    if (lx + LENS_SIZE > window.innerWidth)  lx = e.clientX - LENS_SIZE - offset;
    if (ly + LENS_SIZE > window.innerHeight) ly = e.clientY - LENS_SIZE - offset;
    lens.style.left = lx + 'px';
    lens.style.top  = ly + 'px';
  });
}

function setupLightbox() {
  const lb    = document.getElementById('lightbox');
  const lbImg = document.getElementById('lightbox-img');
  const lbLbl = document.getElementById('lightbox-label');
  if (!lb) return;

  const open = img => {
    lbImg.src = img.src;
    lbLbl.textContent = img.closest('.item').querySelector('h2').textContent;
    lb.classList.add('open');
  };
  const close = () => lb.classList.remove('open');

  document.getElementById('grid').addEventListener('click', e => {
    const img = e.target.closest('.item img');
    if (img) open(img);
  });
  lb.addEventListener('click', close);
  document.addEventListener('keydown', e => { if (e.key === 'Escape') close(); });
}
