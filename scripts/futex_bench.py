#! /usr/bin/python

"""Futex Benchmark

Usage:
    futex_bench.py <num_workers> <num_samples>

Options:
    -h --help       Show this screen.
    --version       Show version.
"""

from docopt import docopt
import atexit
import os
import time
import subprocess

dispatch = None
workers = list()

def exit_handler():
    global dispatch
    global workers

    if dispatch is not None:
        dispatch.poll()
        if dispatch.returncode is None:
            dispatch.kill()

    for w in workers:
        w.poll()
        if w.returncode is None:
            w.kill()


def main(bin_dir, num_workers, num_samples):
    global dispatch
    global workers

    dispatch = subprocess.Popen([bin_dir + '/dispatch', num_workers, num_samples])
    time.sleep(1)
    for i in xrange(int(num_workers)):
        workers.append(subprocess.Popen([bin_dir + '/worker']))
    
    while dispatch.returncode is None:
        time.sleep(1)
        dispatch.poll()
        for w in workers:
            w.poll()
            if w.returncode is not None:
                return

if __name__ == "__main__":
    args = docopt(__doc__, version='Futex Benchmark 0.1')
    project_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
    bin_dir = project_dir + '/bin'
    atexit.register(exit_handler)
    main(bin_dir, args['<num_workers>'], args['<num_samples>'])