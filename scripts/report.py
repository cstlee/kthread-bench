#! /usr/bin/python

"""Report Generator

Usage:
    report.py summary <input_file>

Options:
    -h --help       Show this screen.
    --version       Show version.
"""

from docopt import docopt
import math

def load(filename):
    numbers = []
    with open(filename, 'r') as f:
        for line in f.readlines():
            numbers += [int(i) for i in line.rstrip(',\n').split(',')]
    return numbers

def summary(filename, prefix, unit):
    numbers = load(filename)
    numbers.sort()
    count = len(numbers)
    print("{0:20} {1:>15} {2}".format(prefix + "", numbers[int(math.floor(0.5 * count))], unit))
    print("{0:20} {1:>15} {2}".format(prefix + ".min", numbers[0], unit))
    print("{0:20} {1:>15} {2}".format(prefix + ".9", numbers[int(math.floor(0.9 * count))], unit))
    print("{0:20} {1:>15} {2}".format(prefix + ".99", numbers[int(math.floor(0.99 * count))], unit))
    print("{0:20} {1:>15} {2}".format(prefix + ".999", numbers[int(math.floor(0.999 * count))], unit))

if __name__ == "__main__":
    args = docopt(__doc__, version='Report Generator 0.1')
    if args['summary']:
        summary(args['<input_file>'], 'futex.latency', 'ns')
