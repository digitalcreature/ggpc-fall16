#!/usr/bin/env python

from sys import stdin, stdout
import re

# Author: Tanner Grehawick
# Email: tgrehawi@gmail.com
# Problem 3: Navigating The Flash

class Pair(object):

	__pattern = re.compile(r'\s*(\d+)\s+(\d+)')
	__slots__ = ('r', 'c')

	def __init__(self, r, c):
		self.r = int(r) or 0
		self.c = int(c) or 0

	@classmethod
	def read(cls, file):
		line = file.readline()
		m = cls.__pattern.match(line)
		if m:
			return cls(*m.group(1, 2))

	def __repr__(self):
		return "(%d, %d)" % (self.r, self.c)

class Grid:

	__pattern = re.compile(r'(\d+)')

	def __init__(self, rcount, ccount, rowproducer = None):
		rp = rowproducer
		self.rows = [
			rp and rp(r) or [None for c in xrange(ccount)]
			for r in xrange(rcount)
		]

	@classmethod
	def read(cls, file):
		size = Pair.read(file)
		return cls(
			size.r, size.c,
			lambda r: map(
				int, cls.__pattern.findall(file.readline())
			)
		)

	def __repr__(self):
		return '\n'.join(
			repr(row) for row in self.rows
		)

grid = Grid.read(stdin)
start = Pair.read(stdin)
ends = []
while True:
	end = Pair.read(stdin)
	if end:
		ends.append(end)
	else:
		break
print "grid:\t", repr(grid).replace("\n", "\n\t")
print "start:\t", start
print "ends:",
for end in ends:
	print "\t", end
