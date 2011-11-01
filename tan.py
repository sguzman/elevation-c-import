import sys

titles = []
current = None
count = 0
for l in sys.stdin:
    if l.startswith('    <title>'):
        app = (current, count)
        print current, count
        titles.append(app);
        count = 0;
        current = l[11:-9]
    else:
        count = count + 1

app = (current, count)
titles.append(app);
print current, count


titles.sort(cmp=lambda a,b: b[1]-a[1])

print titles[:90]

s = sum((pd[1] for pd in titles))

print "Revs=%d, pages=%d, avg=%f" % (s, len(titles), float(s)/len(titles))
