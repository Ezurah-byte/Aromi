#gunicorn_config.py
import os

cpu_cores = os.cpu_count()
workers_per_core = float(os.environ.get('GUNICORN_WORKERS_PER_CORE', '0.5')) 
max_workers = int(os.environ.get('GUNICORN_MAX_WORKERS', cpu_cores))
workers = min(max_workers, max(2, int(workers_per_core * cpu_cores)))

threads_per_worker = max(1, int(os.environ.get('GUNICORN_THREADS_PER_WORKER', cpu_cores)))

bind = os.environ.get('GUNICORN_BIND', '127.0.0.1:5010')

timeout = int(os.environ.get('GUNICORN_TIMEOUT', 90)) 

forwarded_allow_ips = '127.0.0.1'

secure_scheme_headers = { 'X-Forwarded-Proto': 'https' } 