#!usr/bin/env python3.4
import itertools,math
balanced_pair_count=0
minimum=1000 #sufficiently large
count=0
omega=0.0
gleft=[]
gright=[]
def check_balance(left,right):
	global count
	global omega
	global balanced_pair_count
	count+=1
	local_omega=min(sum(left),sum(right))/max(sum(left),sum(right))
	if omega <= local_omega:
		balanced_pair_count+=1

def check_min(left,right):
	global gleft
	global gright
	global minimum
	localmin=abs(sum(left)-sum(right))
	if minimum>localmin:
		minimum=localmin
		gleft=left
		gright=right

#Generate a left and right pair for a given permutation
def nuclearwar(perm):
	left_set=[]
	right_set=[]
	for i in perm:
		if sum(right_set)>=sum(left_set):
			left_set.append(i)
		else:
			right_set.append(i)
		check_min(left_set,right_set)
		check_balance(left_set,right_set)
	

if __name__ == '__main__':
	num_elements=int(input())
	omega=float(input())
	number_list=set()
	for i in range(0,num_elements):
		number_list.add(int(input()))
	for i in itertools.permutations(number_list):
		nuclearwar(i)
	print(omega)
	print(gleft)
	print(gright)
	print(minimum)
	print(balanced_pair_count)
	print(count)