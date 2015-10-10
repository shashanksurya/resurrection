#!/usr/bin/env python3.4
def process(fil):
	top=[]
	movie=[0,1,2]
	f=open(fil,"r")
	ratings=[[]]
	count=2500
	for l in f.readlines():
		x=l.split(" ")
		if int(x[2])>=4:
			top.append(int(x[1]))
	for x in top:
		temp=top.count(x)

x=input()
process(x)


