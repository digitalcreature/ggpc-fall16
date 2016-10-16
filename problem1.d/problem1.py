from sys import stdin, stdout
import re
class Segment:
	def __init__(this, a, b):
		a, b = int(a), int(b)
		this.a = min(a, b)
		this.b = max(a, b)
	def __len__(this):
		return this.b - this.a
	def union(this, that):
		if this.b >= that.a and this.a <= that.b:
			return Segment(min(this.a, that.a), max(this.b, that.b))
segs = []
p = re.compile(r'\s*(\d+)\s+(\d+)')
def printsum():
	sum = 0
	for seg in segs:
		sum += len(seg)
	print(sum)
for line in stdin:
	if line == '\n' and len(segs) > 0:
		printsum()
		segs = []
	m = p.match(line)
	if m:
		seg_new = Segment(*m.group(1, 2))
		segs_new = []
		for seg in segs:
			union = seg_new.union(seg)
			if union:
				seg_new = union
			else:
				segs_new.append(seg)
		segs_new.append(seg_new)
		segs = segs_new
printsum()
