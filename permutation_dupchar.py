#!/usr/bin/env python3.4
import time
def permutations(word):
   if len(word)<=1:
       return [word]

   #get all permutations of length N-1
   perms=permutations(word[1:])
   char=word[0]
   result=[]
   #iterate over all permutations of length N-1
   #print(perms)
   #print(char)
   for perm in perms:
       #insert the character into every possible location
       for i in range(len(perm)+1):
           #print("perm i-> "+perm[i:]+" perm :i-> "+perm[:i])
           result.append(perm[i:] + char + perm[:i])
   return result

def permute2(s, next_fixable, char_number, begin, end):
    if end == begin + 1:
        yield "".join(s) # join the characters to form a string
    else:
        for i in range(begin, end):
            if next_fixable[s[i]] == char_number[i]:
                next_fixable[s[i]] += 1
                char_number[begin], char_number[i] = char_number[i], char_number[begin]
                s[begin], s[i] = s[i], s[begin]
                for p in permute2(s, next_fixable, char_number, begin + 1, end):
                    yield p
                s[begin], s[i] = s[i], s[begin]
                char_number[begin], char_number[i] = char_number[i], char_number[begin]
                next_fixable[s[i]] -= 1

def permutations_wo_duplicates(s):
    alphabet = set(s)
    next_fixable = dict.fromkeys(alphabet, 0)
    count = dict.fromkeys(alphabet, 0)
    char_number = [0] * len(s)
    for i, c in enumerate(s):
        char_number[i] = count[c]
        count[c] += 1
    print("next_fixable is \n")
    print(next_fixable)
    print("char number count is \n")
    print(char_number)
    for p in permute2(list(s), next_fixable, char_number, 0, len(s)):
        yield p


s = "shashankss"
start=time.time()
c1 = time.clock()
A=list(permutations(s))
print("It took "+str(time.time()-start)+ " and cpu time is "+str(time.clock()-c1)+" to complete this way!")
print("%s has %s permutations (counting duplicates)" % (s, len(A)))
print("permutations of these that are unique: %s" % len(set(A)))
start=time.time()
c1 = time.clock()
B = list(permutations_wo_duplicates(s))
print("It took "+str(time.time()-start)+" and cpu time is "+str(time.clock()-c1)+" to complete the optimized way!")
print("%s has %s unique permutations (directly computed)" % (s, len(B)))
#print("The first 10 permutations       :", A[:10])
#print("The first 10 unique permutations:", B[:10])