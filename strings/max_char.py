''' Program to return maximum occuring character in a given input 
* If two or more characters are found, then return first character in 
  alphabetical order.
http://www.geeksforgeeks.org/return-maximum-occurring-character-in-the-input-string/
'''
'''Time Complexity: O(n)'''
def findmaxchar(strinput):
	strdict = {}
	for each in strinput:
		try:
			strdict[each] += 1
		except KeyError:
			strdict[each] = 0
	m = max(strdict.values())
	retchar = 255
	for eachchar in  [k for k,v in strdict.iteritems() if v == m]:
		if ord(eachchar) < retchar:
		   retchar = ord(eachchar) 
	return chr(retchar)

if __name__ == '__main__':
	strinput = raw_input()
	print(findmaxchar(strinput))