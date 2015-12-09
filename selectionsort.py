'''This program is for selection sort'''
#/usr/bin/env python3.4
def selsort(A):
	'''This function takes A as input and returns sorted list'''
	for i in range(0,len(A)):
		key = i
		j = i+1
		while j < len(A):
			if A[key]>A[j]:
				key=j
			j+=1
		temp = A[key]
		A[key] = A[i]
		A[i] = temp
	return A

def test_selsort():
	'''This is a unit test for selection sort'''
	result = selsort([4,7,8,9,0,3,-1])
	flag = 1
	for each in range(0,len(result)-1):
		if result[each] > result[each+1]:
			flag = 0
			break
	assert 1 == flag, "Selection sort Test failed!"


if __name__ == '__main__':
	list1 = [4,7,8,9,0,3,-1]
	print(selsort(list1))