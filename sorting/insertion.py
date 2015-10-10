#!/usr/bin/env python
'''Insertion sort algorithm'''

import time

def sort(items):
	for i in range(0,len(items)):
		j = i
		while j>0 and items[j] < items[j-1]:
			#swap with minimum in the array for ascending order
			temp = items[j]
			items[j] = items[j-1]
			items[j-1] = temp
			j-=1
	return items

if __name__ == '__main__':
	items = [5,9,4,2,0,-1,44,78,64,2004,6,64]
	start = time.time()
	print("Sorted list is "+str(sort(items)))
	print("Time Taken: "+str(time.time() - start))