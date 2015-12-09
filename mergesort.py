''' This program is for Merge Sort'''
#/usr/bin/env python3.4
import math
def test_mergesort():
	'''This is a unit test for merge sort'''
	result = mergesort([4,7,8,9,0,3,-1])
	flag = 1
	for each in range(0,len(result)-1):
		if result[each] > result[each+1]:
			flag = 0
			break
	assert 1 == flag, "Merge sort Test failed!"

def mergesort(A):
	if len(A)<2:
		return A
	mid = int(len(A)/2)
	first = mergesort(A[:mid])
	last = mergesort(A[mid:])
	result=[]
	i=0
	j=0
	while i<len(first) and j<len(last):
		if first[i]<=last[j]:
			result.append(first[i])
			i+=1
		else:
			result.append(last[j])
			j+=1
	result+=first[i:]
	result+=last[j:]
	return result

if __name__ == '__main__':
	listx = [2,6,3,1,5,0]
	print(listx)
	print(mergesort(listx))