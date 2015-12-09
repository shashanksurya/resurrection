'''This program is for insertion sort'''
#/usr/bin/env python3.4
def insort(A):
	'''This function takes A as input and returns sorted list'''
	for i in range(1,len(A)):
		key = A[i]
		j = i-1
		while j>=0 and key<A[j]:
			A[j+1] = A[j]
			j-=1
		A[j+1]=key
	return A

def test_insort():
	'''This is a unit test for insertion sort'''
	result = insort([4,7,8,9,0,3,-1])
	flag = 1
	for each in range(0,len(result)-1):
		if result[each] > result[each+1]:
			flag = 0
			break
	assert 1 == flag, "Insertion sort Test failed!"


if __name__ == '__main__':
	list1 = [4,7,8,9,0,3,-1]
	print(insort(list1))