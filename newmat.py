#!/usr/bin/env python3.4
def procc(fil):
   top=[]
   movie=[0,1,2]
   f=open(fil,"r")
   ratings=[[]]
   count=2500
   for l in f.readlines():
      x=l.split(" ")
      if int[x[2]>=4:
         top.append(int(x[1]))
   
   for x in top:
      temp=top.count(x)
      if temp>movie[0] and movie[0]<=movie[1] and movie[1]<=movie[2]:
         movie[0]=temp
         li.append(x)
   print(li)

