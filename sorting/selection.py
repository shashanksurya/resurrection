#!/usr/bin/env python
'''Selection sort algorithm'''

import time

def sort(items):
	for i in range(0,len(items)):
		j = i + 1
		minimum = i
		while j<len(items):
			if items[j] < items[minimum]:
				minimum = j
			j+=1
		#swap with minimum in the array for ascending order
		temp = items[i]
		items[i] = items[minimum]
		items[minimum] = temp
	return items


if __name__ == '__main__':
	items = [5,9,4,2,0,-1,44,78,64,2004,6,64]
	start = time.time()
	print("Sorted list is "+str(sort(items)))
	print("Time Taken: "+str(time.time() - start))