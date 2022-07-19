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

print(f"{title:<10}", end="")
for i in range(len(labels)):
    if counts[i * 2 + 3] != "0":
        print (f" {labels[i]}: {counts[i * 2 + 3]}/{counts[i * 2 + 2]}", end="")
        
print()
