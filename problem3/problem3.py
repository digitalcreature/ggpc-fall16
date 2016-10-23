#!/usr/bin/env python
from sys import stdin, stdout
from itertools import *
from heapq import heappush, heappop
import re
# Author: Tanner Grehawick
# Email: tgrehawi@gmail.com
# Problem 3: Navigating The Flash
inf = 1e413 # why doesnt python have a builtin infinity constant???
class priorityset():
	def __init__(self):
		self.heap = []
		self.set = set()
	def push(self, item):
		heappush(self.heap, item)
		self.set.add(item)
	def pop(self):
		item = heappop(self.heap)
		self.set.remove(item)
		return item
	@property
	def empty(self):
		return len(self.heap) == 0
	def __contains__(self, item):
		return item in self.set
class Pair(object):
	__pattern = re.compile(r'\s*(\d+)\s+(\d+)')
	__slots__ = ('r', 'c')
	def __init__(self, r, c):
		self.r = int(r or 0)
		self.c = int(c or 0)
	@staticmethod
	def read(file):
		line = file.readline()
		m = Pair.__pattern.match(line)
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
		self.Node._goal = goal
		# gscore = {}
		# self.Node._gscore = gscore
		openset = priorityset()
		for pair in self:
			self[pair].gscore = inf
			self[pair].last = None
		start.gscore = 0
		openset.push(start)
		closedset = set()
		while not openset.empty:
			node = openset.pop()
			print repr(node), node.gscore, node.hscore(), node.fscore()
			if node == goal:
				cost = 0
				while node != start:
					print repr(node), node.gscore
					cost += node.costto(node.last)
					node = node.last
				del self.Node._goal
				# del self.Node._gscore
				return cost
			closedset.add(node)
			for neighbor in node.neighbors():
				if neighbor in closedset:
					continue
				newgscore = node.gscore + node.costto(neighbor)
				if neighbor in openset:
					if neighbor.gscore < newgscore:
						continue
				else:
					openset.push(neighbor)
				neighbor.last = node
				neighbor.gscore = newgscore
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
		# @property
		# def gscore(self):
		# 	return self._gscore.get(self, inf)
		# @gscore.setter
		# def set_gscore(self, score):
		# 	self._gscore[self] = score
		def hscore(self):
			return self.grid.heuristic(self, self._goal)
		def fscore(self):
			return self.gscore + self.hscore()
		def __str__(self):
			return "<%d>" % self.cost
		def __repr__(self):
			return "<%r %d>" % (self.pair, self.cost)
		def __lt__(self, other):
			ret = self.fscore() < other.fscore()
			print repr(self), repr(other), ret
			return ret
		def __eq__(self, other):
			return self.pair == other.pair
		def __ne__(self, other):
			return self.pair != other.pair
		def __hash__(self):
			return hash(self.pair)
if __name__ == "__main__":
	grid = Grid.read(stdin)
	start = Pair.read(stdin)
	goals = []
	while True:
		end = Pair.read(stdin)
		if end:
			goals.append(end)
		else:
			break
	print "grid:\t", str(grid).replace("\n", "\n\t")
	print "start:\t", repr(grid[start])
	for goal in goals:
		print "goal:\t", repr(grid[goal])
		print grid.astar(start, goal)
