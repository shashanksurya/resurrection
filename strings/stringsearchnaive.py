#!/usr/bin/env python
'''String searching naiive way'''
import sys

def findmatch(pattern,text):
	m = len(pattern)
	n = len(text)
	for i in range(0,n-m+1):
		j = 0 
		while j<m and pattern[j] == text[i+j]:
			j+=1
		if j == m: return i
	return -1

if __name__ == '__main__':
	if len(sys.argv) != 3:
		print('Usage: python stringsearchnaive.py <pattern> <text>')
		sys.exit(0)
	pattern = sys.argv[1]
	text = sys.argv[2]
	index = findmatch(pattern,text)
	if index == -1:
		print("Pattern not found in text")
	else:
		print("Pattern found in text at "+str(index))