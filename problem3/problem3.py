#!/usr/bin/env python
from sys import stdin, stdout, argv
from collections import defaultdict
from heapq import heappush, heappop
import re
# Author: Tanner Grehawick
# Email: tgrehawi@gmail.com
# Problem 3: Navigating The Flash
inf = 1e413 # why doesnt python have a builtin infinity constant???
empty = () # empty collection
opt_verbose = "-v" in argv or "--verbose" in argv	#verbose option: -v or --verbose
opt_graphic = "-g" in argv or "--graphic" in argv	#graphic option: -g or --graphic (try it, its cool)
opt_astar = "-a" in argv or "--astar" in argv		#use astar instead of dijkstra: -a or --astar
# simple heap-base set that keeps a sort order
# (i use a set as well so its O(1) instead of O(n)
#  to check if something is in the heap)
class PrioritySet:
	def __init__(self):
		self.heap = []
		self.set = set()
	def push(self, item, priority):
		self.set.add(item)
		heappush(self.heap, (priority, item))
	def pop(self):
		item = heappop(self.heap)[1]
		self.set.remove(item)
		return item
	def __contains__(self, item):
		return item in self.set
	def __len__(self):
		return len(self.set)
# ordered pair (row, column)
class Pair(object):
	__pattern = re.compile(r'\s*(\d+)\s+(\d+)')
	__slots__ = ('r', 'c')
	def __init__(self, r, c):
		self.r = int(r)
		self.c = int(c)
	# parse a Pair from a file stream
	@staticmethod
	def read(file):
		line = file.readline()
		m = Pair.__pattern.match(line)
		if m:
			return Pair(m.group(1), m.group(2))
	def __repr__(self):
		return "({}, {})".format(self.r, self.c)
	# iterate over neighbors of this pair
	def neighbors(self):
		# no diagonal movement
		yield Pair(self.r + 1, self.c)
		yield Pair(self.r - 1, self.c)
		yield Pair(self.r, self.c + 1)
		yield Pair(self.r, self.c - 1)
	def __hash__(self):
		#just throw binops at it, itll work, right?
		return (self.r << 21) ^ ~(~self.c << 5)
	def __eq__(self, other):
		return self.r == other.r and self.c == other.c
	def __ne__(self, other):
		return not (self == other)
# a grid of nodes
# (maps Pair => Grid.Node)
class Grid(dict):
	__pattern = re.compile(r'\d+')
	def __init__(self, rcount, ccount):
		# row and column counts
		self.rcount = int(rcount)
		self.ccount = int(ccount)
	# a single node in the grid
	# represents a square on the heighmap
	class Node:
		def __init__(self, grid, pair, cost):
			self.grid = grid
			self.pair = pair
			# [cost] is the height of the node
			self.cost = int(cost or 0)
		def neighbors(self):
			# only include neighbors that are in the grid
			return (
				self.grid[pair]
				for pair in self.pair.neighbors()
				if pair in self.grid
			)
		def costto(self, other):
			return abs(self.cost - other.cost) + 1
		def __str__(self):
			return "<{}>".format(self.cost)
		def __repr__(self):
			return "<{!r} {}>".format(self.pair, self.cost)
		def __hash__(self):
			return id(self) # shouldnt this be the default implementation? cmon python
			# maybe 3.5 is better...
	# read a grid from a file stream
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
	# heuristic function: "how far is it to [goal] from [node]?"
	# admissable
	def heuristic(self, node, goal):
		# manhattan distance (no diagonal movement)
		return abs(node.pair.r - goal.pair.r) + abs(node.pair.c - goal.pair.c)
	# find the path from start to goal, using the A* algorithm
	# returns the total cost of the cheapest path
	def astar(self, start, goal):
		# convert Pairs into Nodes
		start = self[start]
		goal = self[goal]
		# dictionaries mapping Nodes to other stuff
		prevnode = defaultdict(lambda: None) # keep track of where weve been
		gscore = defaultdict(lambda: inf) # cost of path from start to [node]
		def fscore(node):
			# total cost estimate of path from [start] to [goal] through [node]
			return gscore[node] + self.heuristic(node, goal)
		oset = PrioritySet() # open set: nodes we havent fulle evaluated
		cset = set() # closed set: nodes for which weve fully evaluated
		gscore[start] = 0 # [start] to [start] is a 0 cost trip
		oset.push(start, 0) # start at [start]
		while len(oset) > 0:
			node = oset.pop()
			self.printastar(oset, cset, fscore, start, goal, node) # whats this? hmm
			if node == goal:
				path = set()
				cost = 0
				while node != start:
					path.add(node)
					prev = prevnode[node]
					cost += node.costto(prev)
					node = prev
				path.add(start)
				self.printastar(oset, cset, fscore, start, goal, node, path) # there it is again...
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
	# dijkstra algorithm: map the whole grid at once....
	def dijkstra_start(self, start):
		start = self[start]
		gscore = defaultdict(lambda: inf)
		oset = PrioritySet()
		cset = set()
		gscore[start] = 0
		oset.push(start, 0)
		start.prevnode = None
		while len(oset) > 0:
			node = oset.pop()
			self.printastar(oset, cset, gscore.get, start, None, node)
			cset.add(node)
			for neighbor in node.neighbors():
				if neighbor not in cset:
					gscore_est = gscore[node] + node.costto(neighbor)
					if gscore_est < gscore[neighbor]:
						if neighbor not in oset:
							oset.push(neighbor, gscore_est)
						neighbor.prevnode = node
						gscore[neighbor] = gscore_est
	# ...then get the path to any specific goal
	# this should save time when testing lots of different goals, as you wont need to search the map each time
	# instead, you just follow the [node.prevnode]'s!
	def dijkstra_goal(self, goal):
		node = self[goal]
		path = set()
		cost = 0
		while node.prevnode:
			path.add(node)
			cost += node.costto(node.prevnode)
			node = node.prevnode
		self.printastar(empty, empty, int, node, self[goal], None, path)
		return cost
	if opt_graphic:
		# ah, there it is. prints a graphical representation of the current state of the A* algorithm
		# (only defined if the --graphic flag is passed)
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
		#if the --graphic option wasnt defined, dont print anything
		def printastar(*argv):
			pass
if __name__ == "__main__":
	grid = Grid.read(stdin)
	start = Pair.read(stdin)
	if not opt_astar:
		grid.dijkstra_start(start)
	if opt_verbose:
		print "grid:\t", str(grid).replace("\n", "\n\t")
		print "start:\t", repr(grid[start])
	while True:
		goal = Pair.read(stdin)
		if goal:
			if opt_verbose:
				print "goal:\t", repr(grid[goal])
			if opt_astar:
				print grid.astar(start, goal)
			else:
				print grid.dijkstra_goal(goal)
		else:
			break
