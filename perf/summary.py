import sys

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

total_bytes = 0
total_objs = 0

print(f"{title:<10}", end="")
for i in range(len(labels)):
    print (f"{labels[i]}: {counts[i * 2 + 3]}/{counts[i * 2 + 2]}\t", end="")
    total_objs += int(counts[i * 2 + 3])
    total_bytes += int(counts[i * 2 + 2])

print (f"total: {total_objs}/{total_bytes}")
