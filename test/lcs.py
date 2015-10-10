#!/usr/bin/env python3.4
''' This program is for finding the longest common subsequence using DP'''
def lcsfinder(sequence):
	m = len(sequence[0])
	n = len(sequence[1])
	match=[]
	b = [['z' for x in range(n+1)] for x in range(m+1)]
	c = [[0 for x in range(n+1)] for x in range(m+1)]
	for i in range(1,m+1):
		for j in range(1,n+1):
			if sequence[0][i-1]==sequence[1][j-1]:
				c[i][j] = c[i-1][j-1]+1
				b[i][j]='d'
			elif c[i-1][j] >= c[i][j-1]:
				c[i][j] = c[i-1][j]
				b[i][j] = 'u'
			else:
				c[i][j] = c[i][j-1]
				b[i][j] = 's'
	i=m
	j=n
	while(i>0 and j>0):
		if b[i][j] == 'd':
			match.append(sequence[1][j-1])
			i-=1
			j-=1
		elif b[i][j] == 'u':
			i-=1
		elif b[i][j] == 's':
			j-=1
	return ''.join(reversed(match))

if __name__ == '__main__':
	fname = input("Enter a file name:")
	freader = open(fname,"r+")
	list_lines =[]
	input_lines = []
	for line in freader.readlines():
		input_lines.append(line)
		list_lines.append(lcsfinder(line.strip().split()))
		freader.seek(0)
	freader.close()
	fwriter = open(fname,"w")
	for line in range(len(input_lines)):
		fwriter.write(('%s %s\n' % (input_lines[line].rstrip('\n'),list_lines[line] )))
	fwriter.close()