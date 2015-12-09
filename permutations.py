#!/usr/bin/env python3.4
def permutations(word):
   if len(word)<=1:
       return [word]

   #get all permutations of length N-1
   perms=permutations(word[1:])
   char=word[0]
   result=[]
   #iterate over all permutations of length N-1
   print(perms)
   print(char)
   for perm in perms:
       #insert the character into every possible location
       for i in range(len(perm)+1):
           print("perm i-> "+perm[i:]+" perm :i-> "+perm[:i])
           result.append(perm[i:] + char + perm[:i])
   return result

if __name__ == '__main__':
   s=str(input())
   print(len(permutations(''.join(sorted(s)))))