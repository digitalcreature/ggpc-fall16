#!/usr/bin/env python

from sys import stdin, stdout
import re

# Author: Tanner Grehawick
# Email: tgrehawi@gmail.com
# Problem 3: Navigating The Flash
for line in stdin:
	print map(lambda o: int(o), re.compile(r'(\d+)').findall(line))
exit(0)

class InputError(RuntimeError):
	__init__
class Pair:
	pattern = re.compile(r'\s*(\d+)\s+(\d+)')
	def __init__(this, r, c):
		this.r = int(r) or 0
		this.c = int(c) or 0
	@classmethod
	def read(cls, file):
		line = file.readline()
		if line:
			m = Pair.pattern.match(line)
			if not m:
				raise FormatError()
			return cls(*m.group(1, 2))
		else:
			raise EOFError("")
	def __repr__(this):
		return "(%d, %d)" % (this.r, this.c)
class Grid:
	def __init__(this, rcount, ccount, rowsource = None):
		this.vals = []
		for r in range(0, rcount):
			row = rowsource and rowsource(r) or []
			this.vals.append(row)
	@classmethod
	def read(cls, file):
		size = Pair.read(file)
		grid = cls(size.r, size.c, lambda r: ))
	class __RowReader:
		pattern = re.compile(r'\s*(\d+)')
		def __init__(this, file):
			this.file = file
		def __call__(this, r):
			line = this.file.readline()
			if line:
				return map(lambda o: int(o), this.pattern.findall(line))
			else:
				raise EOFError("")
start = Pair.read(stdin)
ends = []
end = Pair.read(stdin)
try:
	while True:
		ends.append(Pair.read(stdin))
except EOFError:
	pass
print ends
