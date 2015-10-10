def fib(n):
    if n == 0:
        return 0
    elif n == 1:
        return 1
    else:
        return fib(n-1) + fib(n-2)

if __name__ == '__main__':
	print(fib(int(input())))
	except:
			print("Failed")
			fcount+=1
			if(fcount>count):
				break
			time.sleep(3)