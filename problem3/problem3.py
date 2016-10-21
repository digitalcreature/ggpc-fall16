#!/usr/bin/env python
from sys import stdin, stdout
import re
# Author: Tanner Grehawick
# Email: tgrehawi@gmail.com
# Problem 3: Navigating The Flash
class Pair:
	__pattern = re.compile(r'\s*(\d+)\s+(\d+)')
	__slots__ = ('r', 'c')
	def __init__(self, r, c):
		self.r = int(r or 0)
		self.c = int(c or 0)
	@classmethod
	def read(cls, file):
		line = file.readline()
		m = cls.__pattern.match(line)
		if m:
			return cls(*m.group(1, 2))
	def __repr__(self):
		return "(%d, %d)" % (self.r, self.c)
	def neighbors(self):
		yield Pair(self.r + 1, self.c)
		yield Pair(self.r - 1, self.c)
		yield Pair(self.r, self.c + 1)
		yield Pair(self.r, self.c - 1)
class Node(Pair):
	def __init__(self, r, c, height, grid):
		Pair.__init__(self, r, c)
		self.height = int(height or 0)
		self.grid = grid
	def neighbors(self):
		return (self.grid[n] for n in Pair.neighbors(self) if self.grid[n])
	def __str__(self):
		return "<%d>" % self.height
	def __repr__(self):
		return "<%s %d>" % (Pair.__repr__(self), self.height)
class Grid:
	__pattern = re.compile(r'(\d+)')
	def __init__(self, rcount, ccount, rowproducer = None):
		rp = rowproducer
		self.rows = [
			rp and rp(self, r) or [None for c in xrange(ccount)]
			for r in xrange(rcount)
		]
	def __getitem__(self, pair):
		if pair.r in xrange(len(self.rows)):
			row = self.rows[pair.r]
			if pair.c in xrange(len(row)):
				return row[pair.c]
	def nodes(self):
		return (node for row in self.rows for node in row)
	@classmethod
	def read(cls, file):
		size = Pair.read(file)
		def rowproducer(grid, r):
			return [
				Node(r, c, height.group(1), grid)
				for c, height in enumerate(cls.__pattern.finditer(file.readline()))
			]
		return cls(size.r, size.c, rowproducer)

	def __repr__(self):
		return '\n'.join(
			" ".join(str(node) for node in row) for row in self.rows
		)
def main():
	grid = Grid.read(stdin)
	start = Pair.read(stdin)
	ends = []
	while True:
		end = Pair.read(stdin)
		if end:
			ends.append(end)
		else:
			break
	print "grid:\t", str(grid).replace("\n", "\n\t")
	print "start:\t", repr(grid[start])
	print "ends:",
	for end in ends:
		print "\t", repr(grid[end])
if __name__ == "__main__":
	main()
