#!usr/bin/env python3.4
'''
This program tests the lists and loops
'''
l=list()
insert="insert"
for count in range(0,5):#A list(of 3 entries) within a list(of 5 entries)
	l.append([count,insert+str(count),"blah blah"])

newl=list()
insert="insert"
for count in range(0,3):
	temp=list()
	for count2 in range(0,3):
		temp.append([count,insert+str(count2),"blah blah"])
	newl.append(temp)
print("The bad way of printing a 3x3 matrix using lists is "+"\n")
print(newl)
print("The good way of printing a 3x3 matrix using lists is "+"\n")
for count in range(0,len(newl)):
	for count2 in  range(0,len(newl[count])):
		print("[",end=" ")
		for count3 in range(0,len(newl[count][count2])):
			print(newl[count][count2][count3],end=",")
		print("]")
	print("\n")
	
