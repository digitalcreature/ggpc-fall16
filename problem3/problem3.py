#!/usr/bin/env python
from sys import stdin, stdout
from itertools import *
import re
# Author: Tanner Grehawick
# Email: tgrehawi@gmail.com
# Problem 3: Navigating The Flash
class Pair(object):
	_pattern = re.compile(r'\s*(\d+)\s+(\d+)')
	__slots__ = ('r', 'c')
	def __init__(self, r, c):
		self.r = int(r or 0)
		self.c = int(c or 0)
	@staticmethod
	def read(file):
		line = file.readline()
		m = Pair._pattern.match(line)
		if m:
			return Pair(*m.group(1, 2))
	def __repr__(self):
		return "(%d, %d)" % (self.r, self.c)
	def neighbors(self):
		yield Pair(self.r + 1, self.c)
		yield Pair(self.r - 1, self.c)
		yield Pair(self.r, self.c + 1)
		yield Pair(self.r, self.c - 1)
	def __hash__(self):
		return (self.r << 16) ^ (~self.c)
	def __eq__(self, other):
		return self.r == other.r and self.c == other.c
	def __ne__(self, other):
		return not (self == other)
class Grid(dict):
	_pattern = re.compile(r'\d+')
	def __init__(self, rcount, ccount):
		self.rcount = int(rcount)
		self.ccount = int(ccount)
	@staticmethod
	def read(file):
		size = Pair.read(file)
		grid = Grid(size.r, size.c)
		for r in xrange(size.r):
			c = 0
			for cost in Grid._pattern.finditer(file.readline()):
				pair = Pair(r, c)
				grid[pair] = Grid.Node(pair, cost.group(), grid)
				c += 1
		return grid
	def __repr__(self):
		return \
			"\n".join(":| " +
				"  ".join(
					str(self[Pair(r, c)])
					for c in xrange(self.ccount)
				) + " |:"
				for r in xrange(self.rcount)
			)
	class Node:
		def __init__(self, pair, cost, grid):
			self.pair = pair
			self.cost = int(cost or 0)
			self.grid = grid
		def neighbors(self):
			return (
				self.grid[pair]
				for pair in self.pair.neighbors()
				if pair in self.grid
			)
		def __str__(self):
			return "<%d>" % self.cost
		def __repr__(self):
			return "<%r %d>" % (self.pair, self.cost)
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
