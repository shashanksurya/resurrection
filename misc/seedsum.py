'''
   A number X is called a sum seed of another number Y, 
   if we can obtain Y by adding the digits of X to X. 
   For example, 256 = 245 + 2 + 4 + 5. Here 245 is the sum seed of 256. 
   Now given a number, we need to find out all it's sum seeds
   and print it otherwise tell if there are no sum seeds of the given number.
'''
import time

def findrange(n):
	return len(str(n))


def isseed(s,n):
	s_sum = 0
	temp = s
	while temp >= 1:
		s_sum += temp%10
		temp /= 10
	if (s_sum + s) == n:
		return True
	return False


def seedsums(n):
 	ret_list = []
 	rng = findrange(n) * 9
 	for each in range((n-rng),n+1):
 		if isseed(each,n):
 			ret_list.append(each)
 	return ret_list

def bruteforce(n):
	ret_list = []
 	for each in range(0,n+1):
 		if isseed(each,n):
 			ret_list.append(each)
 	return ret_list

if __name__ == "__main__":
	print('Enter the range')
	r = int(raw_input())
	print("Using Heuristic")
	start_time = time.time()
	seeds = seedsums(r)
	print(" ".join([str(x) for x in seeds]))
	print("Time Taken: "+str(time.time() - start_time) +" seconds")
	print("Using Brute Force")
	start_time = time.time()
	seeds = bruteforce(r)
	print(" ".join([str(x) for x in seeds]))
	print("Time Taken: "+str(time.time() - start_time)+" seconds")	