import sys

totals = [0.0 for i in range(14)]
length = 0.0

infile = open(sys.argv[1], 'r')   
for line in infile.readlines():
    times = line.rstrip().split()
    totals = [a + float(b) for a, b in zip(totals, times)]
    length += 1

times = [ "time(system)", "time(process)" ]

objects = [
    [ "conses(bytes)", "conses(consumed)" ],
    [ "except(bytes)", "except(consumed)" ],
    [ "functions(bytes)", "functions(consumed)" ],
    [ "streams(bytes)", "streams(consumed)" ],
    [ "symbols(bytes)", "symbols(consumed)" ],
    [ "vectors(bytes)", "vectors(consumed)" ],
]

time_means = [a / length for a in totals[0:2]]
print("%-20s: %10.2f    %-20s: %10.2f" % (times[0], time_means[0], times[1], time_means[1]))

object_means = [a // length for a in totals[2:]]
for i in range(len(object_means) // 2):
    print("%-20s: %10d    %-20s: %10d" % (objects[i][0], object_means[i * 2], objects[i][1], object_means[i * 2 + 1]))

