import sys

def sum_differences(a,b,c):
	return abs(a-b)+abs(b-c)+abs(c-a)

def find_closest(A,B,C,p,q,r,prev,result):
	#sum of differences		
	if p ==len(A) or q == len(B) or r ==len(C) or prev==0:
		#print p,q,r
		return result
	current = sum_differences(A[p],B[q],C[r])
	if current<prev:
		result = (p,q,r)
	else:
		current = prev
	if A[p] == min(A[p],B[q],C[r]):
		return find_closest(A,B,C,p+1,q,r,current,result)
	elif B[q] == min(A[p],B[q],C[r]):
		return find_closest(A,B,C,p,q+1,r,current,result)
	else:
		return find_closest(A,B,C,p,q,r+1,current,result)


def runtests(A,B,C,testname):
	print "-------------------"+testname+"-------------------------------"
	print A,B,C
	result = ()
	me = find_closest(A,B,C,0,0,0,sys.maxint,result)
	print A[me[0]],B[me[1]],C[me[2]]

if __name__ == '__main__':
	A = [20,24,100]
	B = [2,19,22,79,800]
	C = [10,12,23,24,119]
	runtests(A,B,C,"test1")
	A = [1,4,10]
	B = [2,15,20]
	C = [10,12]
	runtests(A,B,C,"test2")
	A = [1,2,3,4,8,10]
	B = [8,10,22,32,44]
	C = [28,32,44,50]
	runtests(A,B,C,"test3")