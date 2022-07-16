#! /usr/bin/env python2
import sys

def main():

    totals = [0.0 for i in range(14)]
    length = 0.0

    infile = open(sys.argv[1], 'r')   
    for line in infile.readlines():
       times = line.rstrip().split()
       totals = [a + float(b) for a, b in zip(totals, times)]
       length += 1

    labels = [
        "time(system)", "time(process)",
        "conses(bytes)", "conses(consumed)",
        "except(bytes)", "except(consumed)",
        "functions(bytes)", "functions(consumed)",
        "streams(bytes)", "streams(consumed)",
        "symbols(bytes)", "symbols(consumed)",
        "vectors(bytes)", "vectors(consumed)",
    ]

    averages = [a / length for a in totals]

    for i in range(int(14 / 2)):
        print("%-20s: %10.2f    %-20s: %10.2f" % (labels[i * 2], averages[i * 2], labels[i * 2 + 1], averages[i * 2 + 1]))

main()
