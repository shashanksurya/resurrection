#!/usr/bin/env python3.4
class adder(value):
	def __init__(self, value=0):
		self.data = value
	def __add__(self, other):
		self.data += other

class addboth(adder):
	def __repr__(self):
		return 'addboth(%s)' % self.data
	def __str__(self):
		return '[Value: %s]' % self.data

if __name__ == '__main__':
	x= addboth(4)
	x+=1
	print(x)
	