#!/usr/bin/env python3.4
x="0 2 0 1 1 2 2 0 1 2 2 0 1 2 2 2 0 0 0 1 1 2 2 0 1 2 0 1 1 2 2 1 1 2 2 0 1 0 0 1 1 2 2 1 0 2 2 2 2"
y="2 0 2 1 1 0 0 2 1 0 0 1 1 0 0 0 2 2 2 2 1 0 0 2 1 0 1 1 1 0 0 1 1 0 2 2 1 2 2 2 1 2 2 1 1 0 0 0 2"
data1=[ str(m.strip('\t\n\r')) for m in x.split()]
data2=[ str(m.strip('\t\n\r')) for m in y.split()]
print(data1)
print(data2)
count=0
for each in range(0,len(data1)-1):
	if data1[each]==data1[each+1] and data2[each]!=data2[each+1]:
		count+=1
	elif data1[each]!=data1[each+1] and data2[each]==data2[each+1]:
		count+=1
print(count)