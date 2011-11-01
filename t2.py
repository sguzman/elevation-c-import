from sys import stdin
from collections import defaultdict  

bins = defaultdict(lambda: 0)

for l in stdin:
    if l.startswith("[('"):
        break
    _, revcount = l.rsplit(None, 1)
    bins[revcount] = bins[revcount] + 1

l = list(bins.iteritems())
l.sort(key=lambda x:int(x[0]))
for r,c in l:
    print r,c
