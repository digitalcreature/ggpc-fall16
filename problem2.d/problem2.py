#!/usr/bin/env python

from sys import stdin, stdout
from inspect import isclass
import re

# Author: Tanner Grehawick
# Email: tgrehawi@gmail.com
# Problem 2: Stack-based Computer Architecture Simulation

class Program:
	p_instruction = re.compile(r'\s*([A-Z]+)\s+([+-]?\w*)')
	p_label = re.compile(r'\s*(\w+):')
	def __init__(this):
		this.instructions = []
		this.labels = {}
	def load(this, srcfile):
		iclasses = {}	# dictionary mapping mnemonics => Instruction subclasses
		for name, cls in globals().items():
			if cls != Instruction and isclass(cls) and issubclass(cls, Instruction):
				iclasses[name] = cls
		for line in srcfile:
			m = this.p_instruction.match(line)
			if m:
				name = m.group(1)
				if iclasses[name]:
					arg = m.group(2)
					if arg:
						try:
							arg = int(arg)
						except:
							pass
					else:
						arg = None
					this.add(iclasses[name](arg))
			else:
				m = this.p_label.match(line)
				if m:
					this.add(Label(m.group(1)))
	def add(this, item):
		if isinstance(item, Instruction):
			item.address = len(this.instructions)
			this.instructions.append(item)
		elif isinstance(item, Label):
			item.address = len(this.instructions)
			this.labels[item.name] = item;
	def link(this):
		for instruction in this.instructions:
			if isinstance(instruction.arg, basestring):
				# if the argument is a string, take it as the name of a label
				instruction.arg = this.labels[instruction.arg]
	def run(this):
		ip = 0		# instruction pointer
		stack = []	# stack (len(stack) gives the stack pointer)
		while ip >= 0 and ip < len(this.instructions):
			instruction = this.instructions[ip]
			# instructions, when called, can return a new instruction pointer.
			# if they do not, the pointer is simply incremented
			ip = instruction.execute(stack) or ip + 1
class Label:
	address = None	# the address this label points to
	def __init__(this, name):
		this.name = name
class Instruction:
	address = None	# the address at which this instruction occurs
	def __init__(this, arg=None):
		this.arg = arg
class PUSH(Instruction):
	def execute(this, stack):
		stack.append(int(this.arg))
class POP(Instruction):
	def execute(this, stack):
		stack.pop()
class DUPE(Instruction):
	def execute(this, stack):
		stack.append(stack[len(stack) - 1])
class PEEK(Instruction):
	def execute(this, stack):
		print(stack[len(stack) - 1])
class ADD(Instruction):
	def execute(this, stack):
		a = stack.pop()
		b = stack.pop()
		stack.append(a + b)
class SUB(Instruction):
	def execute(this, stack):
		a = stack.pop()
		b = stack.pop()
		stack.append(a - b)
class MUL(Instruction):
	def execute(this, stack):
		a = stack.pop()
		b = stack.pop()
		stack.append(a * b)
class DIV(Instruction):
	def execute(this, stack):
		a = stack.pop()
		b = stack.pop()
		stack.append(a // b)
class ROT(Instruction):
	def execute(this, stack):
		stack.insert(len(stack) - int(this.arg), stack.pop())
class BRANCH(Instruction):
	def execute(this, stack):
		return this.arg.address
class CALL(Instruction):
	def execute(this, stack):
		stack.append(this.address + 1)
		return this.arg.address
class BRZ(Instruction):
	def execute(this, stack):
		if stack[len(stack) - 1] == 0:
			return this.arg.address
class BLZ(Instruction):
	def execute(this, stack):
		if stack[len(stack) - 1] < 0:
			return this.arg.address
class BGZ(Instruction):
	def execute(this, stack):
		if stack[len(stack) - 1] > 0:
			return this.arg.address
class RETURN(Instruction):
	def execute(this, stack):
		return stack.pop()
class HCF(Instruction):
	def execute(this, stack):
		return -1

program = Program()

program.load(stdin)
program.link()
program.run()
