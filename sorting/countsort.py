#!/usr/bin/env python
import sys
def counting_sort(il,sl,k):
	B = []
	C = []
	D = [] #for strings
	for i in range(0,len(il)):
		B.append(0)
		D.append('')
	for i in range(0,k+1):
		C.append(0)
	for i in range(0,len(il)):
		C[il[i]]+=1
	for i in range(1,k+1):
		C[i] = C[i-1]+C[i]
	for i in range((len(il)-1),-1,-1):
		temp1 = il[i]
		temp2 = C[temp1]-1
		temp3 = sl[i]
		B[temp2] = temp1
		D[temp2] = temp3
		C[temp1]-=1
	return [B,D]


if __name__ == '__main__':
	fp = open(sys.argv[1],"r")
	intlist = []
	strlist = []
	for eachline in fp.readlines():
		temp = eachline.split()
		intlist.append(int(temp[0]))
		strlist.append(temp[1])
	print(counting_sort(intlist,strlist,max(intlist)))
