#!/usr/bin/env python
from sys import stdin, stdout, argv
# from itertools import *
from collections import defaultdict
from heapq import heappush, heappop
import re
# Author: Tanner Grehawick
# Email: tgrehawi@gmail.com
# Problem 3: Navigating The Flash
inf = 1e413 # why doesnt python have a builtin infinity constant???
opt_verbose = "-v" in argv or "--verbose" in argv
opt_graphic = "-g" in argv or "--graphic" in argv
class Pair(object):
	__pattern = re.compile(r'\s*(\d+)\s+(\d+)')
	__slots__ = ('r', 'c')
	def __init__(self, r, c):
		self.r = int(r)
		self.c = int(c)
	@staticmethod
	def read(file):
		line = file.readline()
		m = Pair.__pattern.match(line)
		if m:
			return Pair(m.group(1), m.group(2))
	def __repr__(self):
		return "(%d, %d)" % (self.r, self.c)
	def neighbors(self):
		yield Pair(self.r + 1, self.c)
		yield Pair(self.r - 1, self.c)
		yield Pair(self.r, self.c + 1)
		yield Pair(self.r, self.c - 1)
	def __hash__(self):
		return (self.r << 21) ^ ~(~self.c << 5)
	def __eq__(self, other):
		return self.r == other.r and self.c == other.c
	def __ne__(self, other):
		return not (self == other)
class Grid(dict):
	__pattern = re.compile(r'\d+')
	def __init__(self, rcount, ccount):
		self.rcount = int(rcount)
		self.ccount = int(ccount)
	@staticmethod
	def read(file):
		size = Pair.read(file)
		grid = Grid(size.r, size.c)
		for r in xrange(size.r):
			c = 0
			for cost in Grid.__pattern.finditer(file.readline()):
				pair = Pair(r, c)
				grid[pair] = grid.Node(grid, pair, cost.group())
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
	def heuristic(self, node, goal):
		return abs(node.pair.r - goal.pair.r) + abs(node.pair.c - goal.pair.c)
	def astar(self, start, goal):
		start = self[start]
		goal = self[goal]
		prevnode = defaultdict(lambda: None)
		gscore = defaultdict(lambda: inf)
		def fscore(node):
			return gscore[node] + self.heuristic(node, goal)
		oset = self.OpenSet()
		cset = set()
		gscore[start] = 0
		oset.push(start, 0)
		while len(oset) > 0:
			node = oset.pop()
			self.printastar(oset, cset, fscore, start, goal, node)
			if node == goal:
				path = set()
				cost = 0
				while node != start:
					path.add(node)
					prev = prevnode[node]
					cost += node.costto(prev)
					node = prev
				path.add(start)
				self.printastar(oset, cset, fscore, start, goal, node, path)
				return cost
			cset.add(node)
			for neighbor in node.neighbors():
				if neighbor not in cset:
					gscore_est = gscore[node] + node.costto(neighbor)
					if gscore_est < gscore[neighbor]:
						if neighbor not in oset:
							oset.push(neighbor, gscore_est)
						prevnode[neighbor] = node
						gscore[neighbor] = gscore_est
	if opt_graphic:
		def printastar(self, oset, cset, fscore, start, goal, current, path = None):
			print
			reset = "\x1B[0m"
			for r in xrange(self.rcount):
				for c in xrange(self.ccount):
					stdout.write(reset),
					node = self[Pair(r, c)]
					color = ""
					text = node.cost
					if start == node:
						color = "\x1B[7;34m"
						text = "starta"
					elif goal == node:
						color = "\x1B[7;35m"
						text = "goal"
					elif path and node in path:
						color = "\x1B[7;32m"
						text = node.cost
					elif node in cset:
						color = color or "\x1B[7;31m"
						text = fscore(node)
					elif node in oset:
						color = color or "\x1B[7;33m"
						text = fscore(node)
					elif node == current:
						color = color or "\x1B[7;37m"
						text = fscore(node)
					stdout.write(color + "{:^5}".format(text)[:5])
				print reset
	else:
		def printastar(*argv):
			pass
	class OpenSet:
		def __init__(self):
			self.heap = []
			self.set = set()
		def push(self, node, fscore):
			self.set.add(node)
			heappush(self.heap, (fscore, node))
		def pop(self):
			node = heappop(self.heap)[1]
			self.set.remove(node)
			return node
		def __contains__(self, node):
			return node in self.set
		def __len__(self):
			return len(self.set)
	class Node:
		def __init__(self, grid, pair, cost):
			self.grid = grid
			self.pair = pair
			self.cost = int(cost or 0)
		def neighbors(self):
			return (
				self.grid[pair]
				for pair in self.pair.neighbors()
				if pair in self.grid
			)
		def costto(self, other):
			return abs(self.cost - other.cost) + 1
		def __str__(self):
			return "<%d>" % self.cost
		def __repr__(self):
			return "<%r %d>" % (self.pair, self.cost)
		def __hash__(self):
			return id(self)
if __name__ == "__main__":
	grid = Grid.read(stdin)
	start = Pair.read(stdin)
	if opt_verbose:
		print "grid:\t", str(grid).replace("\n", "\n\t")
		print "start:\t", repr(grid[start])
	while True:
		goal = Pair.read(stdin)
		if goal:
			if opt_verbose:
				print "goal:\t", repr(grid[goal])
			print grid.astar(start, goal)
		else:
			break
