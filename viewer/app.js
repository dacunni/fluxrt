// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

const RENDER_TITLE = 'Render Results';
const IMAGE_BASE   = '../build/app/';

// Human-readable labels for known output filenames. Any discovered file
// not listed here will have its label derived from its filename.
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

const ZOOM      = 4;
const LENS_SIZE = 200;

// ---------------------------------------------------------------------------
// Image discovery
// ---------------------------------------------------------------------------

function labelFor(filename) {
  return LABELS[filename] || filename.replace(/\.png$/i, '').replace(/[_-]/g, ' ');
}

async function discoverImages() {
  const res  = await fetch(IMAGE_BASE);
  const html = await res.text();
  const doc  = new DOMParser().parseFromString(html, 'text/html');
  return [...doc.querySelectorAll('a[href]')]
    .map(a => a.getAttribute('href'))
    .filter(href => href.toLowerCase().endsWith('.png'))
    .map(file => ({ file, label: labelFor(file) }));
}

// ---------------------------------------------------------------------------
// Build image grid
// ---------------------------------------------------------------------------

function buildGrid(images) {
  const grid = document.getElementById('grid');
  grid.innerHTML = '';
  images.forEach(({ file, label }) => {
    const item = document.createElement('div');
    item.className = 'item';

    const h2 = document.createElement('h2');
    h2.textContent = label;

    const img = document.createElement('img');
    img.src = IMAGE_BASE + file;
    img.alt = label;
    img.onerror = () => img.classList.add('error');

    item.appendChild(h2);
    item.appendChild(img);
    grid.appendChild(item);
  });
}

// ---------------------------------------------------------------------------
// Zoom lens
// ---------------------------------------------------------------------------

function setupLens() {
  const lens   = document.getElementById('lens');
  const canvas = document.getElementById('lens-canvas');
  const ctx    = canvas.getContext('2d');
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
    try {
      ctx.drawImage(img, sx, sy, srcSize, srcSize, 0, 0, LENS_SIZE, LENS_SIZE);
    } catch (_) { /* broken image — leave lens blank */ }

    const offset = 16;
    let lx = e.clientX + offset;
    let ly = e.clientY + offset;
    if (lx + LENS_SIZE > window.innerWidth)  lx = e.clientX - LENS_SIZE - offset;
    if (ly + LENS_SIZE > window.innerHeight) ly = e.clientY - LENS_SIZE - offset;
    lens.style.left = lx + 'px';
    lens.style.top  = ly + 'px';
  });
}

// ---------------------------------------------------------------------------
// Lightbox
// ---------------------------------------------------------------------------

function setupLightbox() {
  const lightbox = document.getElementById('lightbox');
  const lbImg    = document.getElementById('lightbox-img');
  const lbLabel  = document.getElementById('lightbox-label');

  function open(img) {
    lbImg.src           = img.src;
    lbLabel.textContent = img.closest('.item').querySelector('h2').textContent;
    lightbox.classList.add('open');
  }

  function close() {
    lightbox.classList.remove('open');
  }

  document.getElementById('grid').addEventListener('click', e => {
    const img = e.target.closest('.item img');
    if (img) open(img);
  });

  lightbox.addEventListener('click', close);
  document.addEventListener('keydown', e => { if (e.key === 'Escape') close(); });
}

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------

document.getElementById('render-title').textContent = RENDER_TITLE;
setupLens();
setupLightbox();

discoverImages()
  .then(images => {
    if (images.length === 0) {
      document.getElementById('grid').textContent = 'No images found in ' + IMAGE_BASE;
    } else {
      buildGrid(images);
    }
  })
  .catch(err => {
    document.getElementById('grid').textContent = 'Could not load image list: ' + err.message;
  });
