''' Program to remove all duplicates from input string
http://www.geeksforgeeks.org/remove-all-duplicates-from-the-input-string/
'''
'''Time Complexity: O(n)'''
def dupremover(strinput):
	strlist = []
	retstring = ''
	for each in strinput:
		if each not in strlist:
			retstring += each
			strlist.append(each)
	return retstring

if __name__ == '__main__':
	print(dupremover(raw_input()))