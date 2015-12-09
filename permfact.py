#!/usr/bin/env python3.4
'''
This program is for computing permutations and factors
'''
import itertools  

def getnumbers(permutation):
   numberlist=[]
   for each in permutation:
      temp=''
      for numb in each:
         temp+=numb
      numberlist.append(int(temp))
   return numberlist

def getfactors(number):
   factors=[]
   for i in range(1,number):
      if number%i==0:
         factors.append(i)
   return len(factors)

if __name__=="__main__":
   factlist={}
   minval=100
   listminvals=[]
   getdig=input()
   ldigit=getdig.split(" ")
   permutations=list(itertools.permutations(ldigit))
   numberlist=getnumbers(permutations)
   #print(numberlist)
   for each in numberlist:
      factlist[each]=getfactors(each)
   #print(factlist)
   minval=min(value for key,value in factlist.items())
   for key,value in factlist.items():
      if value == minval:
         listminvals.append(key)
   print(min(listminvals))