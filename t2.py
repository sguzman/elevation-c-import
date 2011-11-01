from sys import stdin
from collections import defaultdict  

bins = defaultdict(lambda: 0)

for l in stdin:
    if l.startswith("[('"):
        break
    page, revcount = l.rsplit(None, 1)
    if page.startswith('Benutzer:') or page.startswith('Diskussion:'):
        collect = page[0]+':'+page.split(':')[1][0]
    else:
        collect = page[0]
    bins[collect] = bins[collect] + 1

l = list(bins.iteritems())
l.sort(key=lambda x:x[0])
for r,c in l:
    print r,c
