#! /usr/bin/env python2
import sys

def main():

    totals = [0.0 for i in range(14)]
    length = 0.0

    title = sys.argv[1]
    infile = open(sys.argv[2], 'r')
    line = infile.readline()
    counts = line.rstrip().split()

    labels = [
        "conses",
        "exceptions",
        "functions",
        "streams",
        "symbols",
        "vectors"
    ]

    print '{0:<10}'.format(title),
    for i in range(6):
        if counts[i * 2 + 3] != "0":
            print '{0}({1}/{2})'.format(labels[i], counts[i * 2 + 3], counts[i * 2 + 2]),

main()
