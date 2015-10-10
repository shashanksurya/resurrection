#!/usr/bin/env python3.4
class Complex:
	def __init__(self,real,imaginary):
		self._r = real
		self.i = imaginary
	def __repr__(self):
		return "Imaginary number "+str(self.r)+str(self.i)+"i"

if __name__ == '__main__':
	x = Complex(3.0,-4.5)
	x.r = 2
	print(x._r)