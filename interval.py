#!/usr/bin/env python3.4
''' This program is for interval scheduling using greedy approach'''
def interval_schedule(jobs):
	jobs = sort_short(jobs)
	sel_jobs = []
	n = len(jobs)
	sel_jobs.append(jobs[0][0])
	k = 0
	for m in range(1,n):
		if jobs[m][1]>=jobs[k][2]:
			sel_jobs.append(jobs[m][0])
			k = m
	return sel_jobs

def sort_short(jobs):
	temp =[]
	ret_jobs=[]
	temp.append([x for x in range(0,len(jobs))])
	temp.append([x[2] for x in jobs])
	temp = dict(zip(temp[0],temp[1]))
	for each in sorted(temp,key=temp.get):
		ret_jobs.append(jobs[each])
	print(ret_jobs)
	return ret_jobs

if __name__ == '__main__':
	fname = input("Enter a file name:")
	freader = open(fname,"r")
	jobs =[]
	for line in freader.readlines():
		temp=line.strip().split()
		temp[1]=int(temp[1])
		temp[2]=int(temp[2])
		jobs.append(temp)
	#print(jobs)
	print(interval_schedule(jobs))	