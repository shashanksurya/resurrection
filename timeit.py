import time

def timeit(func):
	def wrapper(*args,**kw):
		start=time.time()
		endtime=time.time()
		diff=endtime-start
		print("Your function took "+str(diff)+" seconds")
		return func(*args,**kw)
	return wrapper

@timeit
def timetakingfunc():
	time.sleep(1)

if __name__ == '__main__':
	timetakingfunc()