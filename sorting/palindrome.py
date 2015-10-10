
x= raw_input("Enter a word")
checker = 0 
for i in range(0,len(x)/2,1):
	if x[i]!=x[len(x)-i-1]:
		checker = 1
		break
if checker == 1:
	print("Its not palindrome")
else:
	print("Its palindrome")
