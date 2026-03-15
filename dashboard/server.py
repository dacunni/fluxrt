#!/usr/bin/env python3
"""FluxRT render dashboard server."""

import glob
import json
import os
import re
import signal
import subprocess
import threading
from datetime import datetime, timezone
from pathlib import Path

# ---------------------------------------------------------------------------
# Paths (auto-detected relative to this file)
# ---------------------------------------------------------------------------

ROOT       = Path(__file__).parent.parent.resolve()
JOBS_DIR   = ROOT / 'jobs'
SCENES_DIR = ROOT / 'scenes'
BINARY     = ROOT / 'build' / 'app' / 'trace_scene'
DASH_DIR   = ROOT / 'dashboard'
VIEWER_DIR = ROOT / 'viewer'

DEFAULT_MESH_PATH    = '/Users/dave/Projects/data/3dmodels'
DEFAULT_ENVMAP_PATH  = '/Users/dave/Projects/data/envmaps'

LABEL_ORDER = [
    'trace_color_tone_mapped.png',
    'trace_color.png',
    'trace_isect_distance.png',
    'trace_hit_mask.png',
    'trace_isect_normal.png',
    'trace_isect_stddev.png',
    'trace_isect_mat_diffuse.png',
    'trace_isect_mat_specular.png',
    'trace_isect_tangent.png',
    'trace_isect_bitangent.png',
    'trace_isect_texcoord.png',
]
LABELS = {
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
}

# ---------------------------------------------------------------------------
# Manifest helpers
# ---------------------------------------------------------------------------

def manifest_path(job_id):
    return JOBS_DIR / job_id / 'manifest.json'

def read_manifest(job_id):
    p = manifest_path(job_id)
    with open(p) as f:
        return json.load(f)

def write_manifest(job_id, data):
    p = manifest_path(job_id)
    tmp = p.with_suffix('.json.tmp')
    tmp.write_text(json.dumps(data, indent=2))
    os.replace(tmp, p)

def list_jobs():
    jobs = []
    if not JOBS_DIR.exists():
        return jobs
    for d in sorted(JOBS_DIR.iterdir(), reverse=True):
        mp = d / 'manifest.json'
        if d.is_dir() and mp.exists():
            try:
                m = json.loads(mp.read_text())
                if m.get('status') == 'running':
                    started = datetime.fromisoformat(m['started_at'])
                    m['elapsed_seconds'] = int((datetime.now() - started).total_seconds())
                else:
                    if m.get('started_at') and m.get('finished_at'):
                        s = datetime.fromisoformat(m['started_at'])
                        e = datetime.fromisoformat(m['finished_at'])
                        m['elapsed_seconds'] = int((e - s).total_seconds())
                    else:
                        m['elapsed_seconds'] = None
                jobs.append(m)
            except Exception:
                pass
    return jobs

def job_images(job_id):
    job_dir = JOBS_DIR / job_id
    found = {p.name for p in job_dir.glob('trace_*.png')}
    # Return in canonical order first, then any extras alphabetically
    images = []
    for f in LABEL_ORDER:
        if f in found:
            images.append({'file': f, 'label': LABELS[f]})
            found.discard(f)
    for f in sorted(found):
        label = f.replace('.png', '').replace('_', ' ')
        images.append({'file': f, 'label': label})
    return images

# ---------------------------------------------------------------------------
# Reconcile pass: fix stale "running" manifests after server restart
# ---------------------------------------------------------------------------

def reconcile():
    JOBS_DIR.mkdir(exist_ok=True)
    for d in JOBS_DIR.iterdir():
        mp = d / 'manifest.json'
        if not (d.is_dir() and mp.exists()):
            continue
        try:
            m = json.loads(mp.read_text())
            if m.get('status') != 'running':
                continue
            pid = m.get('pid')
            alive = False
            if pid:
                try:
                    os.kill(pid, 0)
                    alive = True
                except (ProcessLookupError, PermissionError):
                    pass
            if not alive:
                m['status'] = 'failed'
                m['finished_at'] = datetime.now().isoformat(timespec='seconds')
                write_manifest(m['job_id'], m)
        except Exception:
            pass

# ---------------------------------------------------------------------------
# Job launcher
# ---------------------------------------------------------------------------

def make_job_id(scene):
    ts = datetime.now().strftime('%Y%m%d_%H%M%S')
    stem = Path(scene).stem
    safe = re.sub(r'[^a-zA-Z0-9_-]', '_', stem)[:32]
    return f'{ts}_{safe}'

def build_argv(manifest):
    p = manifest['params']
    args = [
        str(BINARY),
        '-s', str(p['spp']),
        '-t', str(p['threads']),
        '-d', str(p['depth']),
        '-p', str(p['sensor_scale']),
        '-o', p['render_order'],
    ]
    if p.get('flush_timeout', 0) > 0:
        args += ['-f', str(p['flush_timeout'])]
    args.append(str(SCENES_DIR / manifest['scene']))
    return args

def reap_process(job_id, proc, out_fh, err_fh):
    proc.wait()
    out_fh.close()
    err_fh.close()
    try:
        m = read_manifest(job_id)
        if m['status'] == 'running':
            m['status'] = 'done' if proc.returncode == 0 else 'failed'
            m['finished_at'] = datetime.now().isoformat(timespec='seconds')
            m['exit_code'] = proc.returncode
            write_manifest(job_id, m)
    except Exception:
        pass

def launch_job(scene, params):
    job_id = make_job_id(scene)
    job_dir = JOBS_DIR / job_id
    job_dir.mkdir(parents=True)

    manifest = {
        'job_id': job_id,
        'scene': scene,
        'scene_short': Path(scene).stem,
        'status': 'running',
        'started_at': datetime.now().isoformat(timespec='seconds'),
        'finished_at': None,
        'exit_code': None,
        'pid': None,
        'params': params,
    }
    write_manifest(job_id, manifest)

    argv = build_argv(manifest)
    env = os.environ.copy()
    env.setdefault('MESH_PATH',    DEFAULT_MESH_PATH)
    env.setdefault('ENV_MAP_PATH', DEFAULT_ENVMAP_PATH)

    out_fh = open(job_dir / 'output.txt', 'wb')
    err_fh = open(job_dir / 'errors.txt', 'wb')

    proc = subprocess.Popen(
        argv,
        cwd=str(job_dir),
        env=env,
        stdout=out_fh,
        stderr=err_fh,
    )

    manifest['pid'] = proc.pid
    write_manifest(job_id, manifest)

    t = threading.Thread(target=reap_process, args=(job_id, proc, out_fh, err_fh), daemon=True)
    t.start()

    return job_id

# ---------------------------------------------------------------------------
# Flask app
# ---------------------------------------------------------------------------

try:
    from flask import Flask, request, jsonify, send_file, abort, Response
    app = Flask(__name__)

    # -- Static file serving --------------------------------------------------

    @app.route('/')
    def index():
        return send_file(DASH_DIR / 'index.html')

    @app.route('/job/<job_id>')
    def job_page(job_id):
        return send_file(DASH_DIR / 'job.html')

    @app.route('/static/dashboard/<path:filename>')
    def static_dashboard(filename):
        return send_file(DASH_DIR / filename)

    @app.route('/static/viewer/<path:filename>')
    def static_viewer(filename):
        return send_file(VIEWER_DIR / filename)

    @app.route('/jobs/<job_id>/<path:filename>')
    def job_file(job_id, filename):
        p = JOBS_DIR / job_id / filename
        if not p.exists():
            abort(404)
        return send_file(p)

    # -- API ------------------------------------------------------------------

    @app.route('/api/scenes')
    def api_scenes():
        pattern = str(SCENES_DIR / '**' / '*.toml')
        files = sorted(glob.glob(pattern, recursive=True))
        scenes = [str(Path(f).relative_to(SCENES_DIR)) for f in files]
        return jsonify({'scenes': scenes})

    @app.route('/api/jobs', methods=['GET'])
    def api_jobs():
        return jsonify({'jobs': list_jobs()})

    @app.route('/api/jobs', methods=['POST'])
    def api_create_job():
        data = request.get_json(force=True)
        scene = data.get('scene', '').strip()
        if not scene:
            return jsonify({'error': 'scene is required'}), 400
        scene_path = SCENES_DIR / scene
        if not scene_path.exists():
            return jsonify({'error': f'scene not found: {scene}'}), 400
        if not BINARY.exists():
            return jsonify({'error': f'renderer binary not found: {BINARY}'}), 500

        params = {
            'spp':          int(data.get('spp', 10)),
            'threads':      int(data.get('threads', 4)),
            'depth':        int(data.get('depth', 5)),
            'sensor_scale': float(data.get('sensor_scale', 1.0)),
            'render_order': data.get('render_order', 'progressive'),
            'flush_timeout': int(data.get('flush_timeout', 0)),
        }
        job_id = launch_job(scene, params)
        return jsonify({'job_id': job_id}), 201

    @app.route('/api/jobs/<job_id>', methods=['GET'])
    def api_job_detail(job_id):
        try:
            m = read_manifest(job_id)
        except FileNotFoundError:
            abort(404)
        if m.get('status') == 'running':
            started = datetime.fromisoformat(m['started_at'])
            m['elapsed_seconds'] = int((datetime.now() - started).total_seconds())
        elif m.get('started_at') and m.get('finished_at'):
            s = datetime.fromisoformat(m['started_at'])
            e = datetime.fromisoformat(m['finished_at'])
            m['elapsed_seconds'] = int((e - s).total_seconds())
        m['images'] = job_images(job_id)
        return jsonify(m)

    @app.route('/api/jobs/<job_id>/flush', methods=['POST'])
    def api_flush(job_id):
        try:
            m = read_manifest(job_id)
        except FileNotFoundError:
            abort(404)
        if m.get('status') != 'running':
            return jsonify({'error': 'job is not running'}), 409
        pid = m.get('pid')
        if not pid:
            return jsonify({'error': 'no pid recorded'}), 500
        try:
            os.kill(pid, signal.SIGUSR1)
        except ProcessLookupError:
            return jsonify({'error': 'process not found'}), 409
        return jsonify({'ok': True})

    @app.route('/api/jobs/<job_id>/cancel', methods=['POST'])
    def api_cancel(job_id):
        try:
            m = read_manifest(job_id)
        except FileNotFoundError:
            abort(404)
        if m.get('status') != 'running':
            return jsonify({'error': 'job is not running'}), 409
        pid = m.get('pid')
        if pid:
            try:
                os.kill(pid, signal.SIGTERM)
            except ProcessLookupError:
                pass
        m['status'] = 'cancelled'
        m['finished_at'] = datetime.now().isoformat(timespec='seconds')
        write_manifest(job_id, m)
        return jsonify({'ok': True})

    def _read_log(job_id, filename):
        p = JOBS_DIR / job_id / filename
        offset = int(request.args.get('offset', 0))
        limit  = int(request.args.get('limit', 65536))
        total  = p.stat().st_size if p.exists() else 0
        text   = ''
        if p.exists() and offset < total:
            with open(p, 'rb') as f:
                f.seek(offset)
                chunk = f.read(limit)
            text = chunk.decode('utf-8', errors='replace')
        return jsonify({
            'text':        text,
            'offset':      offset,
            'next_offset': offset + len(text.encode('utf-8')),
            'total_bytes': total,
        })

    @app.route('/api/jobs/<job_id>/output')
    def api_output(job_id):
        if not (JOBS_DIR / job_id).exists():
            abort(404)
        return _read_log(job_id, 'output.txt')

    @app.route('/api/jobs/<job_id>/errors')
    def api_errors(job_id):
        if not (JOBS_DIR / job_id).exists():
            abort(404)
        return _read_log(job_id, 'errors.txt')

    # -------------------------------------------------------------------------

    if __name__ == '__main__':
        reconcile()
        print(f'FluxRT Dashboard running at http://localhost:8765/')
        print(f'  Root:   {ROOT}')
        print(f'  Jobs:   {JOBS_DIR}')
        print(f'  Binary: {BINARY}')
        app.run(host='127.0.0.1', port=8765, debug=False, threaded=True)

except ImportError:
    # ---------------------------------------------------------------------------
    # Stdlib fallback (no Flask)
    # ---------------------------------------------------------------------------
    import http.server
    import urllib.parse

    class Handler(http.server.BaseHTTPRequestHandler):
        def log_message(self, fmt, *args):
            pass  # suppress default logging

        def send_json(self, data, status=200):
            body = json.dumps(data).encode()
            self.send_response(status)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Content-Length', len(body))
            self.end_headers()
            self.wfile.write(body)

        def send_file(self, path, content_type='application/octet-stream'):
            path = Path(path)
            if not path.exists():
                self.send_error(404)
                return
            data = path.read_bytes()
            ct = content_type
            if path.suffix == '.html': ct = 'text/html'
            elif path.suffix == '.css':  ct = 'text/css'
            elif path.suffix == '.js':   ct = 'application/javascript'
            elif path.suffix == '.png':  ct = 'image/png'
            elif path.suffix == '.json': ct = 'application/json'
            self.send_response(200)
            self.send_header('Content-Type', ct)
            self.send_header('Content-Length', len(data))
            self.end_headers()
            self.wfile.write(data)

        def read_body(self):
            length = int(self.headers.get('Content-Length', 0))
            return json.loads(self.rfile.read(length)) if length else {}

        def do_GET(self):
            parsed = urllib.parse.urlparse(self.path)
            path = parsed.path
            qs = dict(urllib.parse.parse_qsl(parsed.query))

            if path == '/':
                self.send_file(DASH_DIR / 'index.html')
            elif path.startswith('/job/'):
                self.send_file(DASH_DIR / 'job.html')
            elif path.startswith('/static/dashboard/'):
                self.send_file(DASH_DIR / path[len('/static/dashboard/'):])
            elif path.startswith('/static/viewer/'):
                self.send_file(VIEWER_DIR / path[len('/static/viewer/'):])
            elif path.startswith('/jobs/'):
                parts = path.split('/', 3)
                if len(parts) >= 4:
                    self.send_file(JOBS_DIR / parts[2] / parts[3])
                else:
                    self.send_error(404)
            elif path == '/api/scenes':
                files = sorted(glob.glob(str(SCENES_DIR / '**' / '*.toml'), recursive=True))
                scenes = [str(Path(f).relative_to(SCENES_DIR)) for f in files]
                self.send_json({'scenes': scenes})
            elif path == '/api/jobs':
                self.send_json({'jobs': list_jobs()})
            elif re.match(r'^/api/jobs/[^/]+$', path):
                job_id = path.split('/')[-1]
                mp = manifest_path(job_id)
                if not mp.exists():
                    self.send_error(404); return
                m = read_manifest(job_id)
                if m.get('status') == 'running':
                    s = datetime.fromisoformat(m['started_at'])
                    m['elapsed_seconds'] = int((datetime.now() - s).total_seconds())
                m['images'] = job_images(job_id)
                self.send_json(m)
            elif re.match(r'^/api/jobs/[^/]+/output$', path):
                job_id = path.split('/')[-2]
                if not (JOBS_DIR / job_id).exists():
                    self.send_error(404); return
                offset = int(qs.get('offset', 0))
                limit  = int(qs.get('limit', 65536))
                p = JOBS_DIR / job_id / 'output.txt'
                total = p.stat().st_size if p.exists() else 0
                text = ''
                if p.exists() and offset < total:
                    with open(p, 'rb') as f:
                        f.seek(offset)
                        text = f.read(limit).decode('utf-8', errors='replace')
                self.send_json({'text': text, 'offset': offset,
                                'next_offset': offset + len(text.encode()), 'total_bytes': total})
            elif re.match(r'^/api/jobs/[^/]+/errors$', path):
                job_id = path.split('/')[-2]
                if not (JOBS_DIR / job_id).exists():
                    self.send_error(404); return
                offset = int(qs.get('offset', 0))
                limit  = int(qs.get('limit', 65536))
                p = JOBS_DIR / job_id / 'errors.txt'
                total = p.stat().st_size if p.exists() else 0
                text = ''
                if p.exists() and offset < total:
                    with open(p, 'rb') as f:
                        f.seek(offset)
                        text = f.read(limit).decode('utf-8', errors='replace')
                self.send_json({'text': text, 'offset': offset,
                                'next_offset': offset + len(text.encode()), 'total_bytes': total})
            else:
                self.send_error(404)

        def do_POST(self):
            path = self.path.split('?')[0]
            if path == '/api/jobs':
                data = self.read_body()
                scene = data.get('scene', '').strip()
                if not scene or not (SCENES_DIR / scene).exists():
                    self.send_json({'error': 'invalid scene'}, 400); return
                if not BINARY.exists():
                    self.send_json({'error': 'binary not found'}, 500); return
                params = {
                    'spp': int(data.get('spp', 10)), 'threads': int(data.get('threads', 4)),
                    'depth': int(data.get('depth', 5)), 'sensor_scale': float(data.get('sensor_scale', 1.0)),
                    'render_order': data.get('render_order', 'progressive'),
                    'flush_timeout': int(data.get('flush_timeout', 0)),
                }
                job_id = launch_job(scene, params)
                self.send_json({'job_id': job_id}, 201)
            elif re.match(r'^/api/jobs/[^/]+/flush$', path):
                job_id = path.split('/')[-2]
                mp = manifest_path(job_id)
                if not mp.exists():
                    self.send_error(404); return
                m = read_manifest(job_id)
                if m.get('status') != 'running':
                    self.send_json({'error': 'not running'}, 409); return
                try:
                    os.kill(m['pid'], signal.SIGUSR1)
                    self.send_json({'ok': True})
                except ProcessLookupError:
                    self.send_json({'error': 'process not found'}, 409)
            elif re.match(r'^/api/jobs/[^/]+/cancel$', path):
                job_id = path.split('/')[-2]
                mp = manifest_path(job_id)
                if not mp.exists():
                    self.send_error(404); return
                m = read_manifest(job_id)
                if m.get('status') != 'running':
                    self.send_json({'error': 'not running'}, 409); return
                try:
                    os.kill(m['pid'], signal.SIGTERM)
                except ProcessLookupError:
                    pass
                m['status'] = 'cancelled'
                m['finished_at'] = datetime.now().isoformat(timespec='seconds')
                write_manifest(job_id, m)
                self.send_json({'ok': True})
            else:
                self.send_error(404)

    if __name__ == '__main__':
        reconcile()
        print('Flask not found — using stdlib fallback')
        print(f'FluxRT Dashboard running at http://localhost:8765/')
        server = http.server.HTTPServer(('127.0.0.1', 8765), Handler)
        server.serve_forever()
