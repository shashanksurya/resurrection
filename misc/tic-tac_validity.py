'''
  A Tic-Tac-Toe board is given after some moves are played. 
  Find out if the given board is valid, i.e., is it possible to reach this
  board position after some moves or not.
  Testcase1: [['X', 'X', 'O'], 
              ['O', 'O', 'X'],
              ['X', 'O', 'X']]
  Valid
  Testcase2: [['O', 'X', 'X'], 
              ['O', 'X', 'X'],
              ['O', 'O', 'X']]
  Invalid
  Testcase3: [['O', 'X', ''], 
              ['', '', ''],
              ['', '', '']]
  Valid
  Testcase4: [['X', 'X', 'X'], 
              ['', 'O', ''],
              ['', 'O', 'O']]
  Invalid
 '''

def diagsum(matrix):
	diagsums = []
	diagsums.append(matrix[0][0]+matrix[1][1]+matrix[2][2])
	diagsums.append(matrix[0][2]+matrix[1][1]+matrix[2][0])
	return diagsums

def checkwinning(integer_tictac):
	rowsum = [sum(row) for row in integer_tictac]
	colsum = [sum(col) for col in zip(*integer_tictac)]
	diagsums = diagsum(integer_tictac)
	check = 0
	win = ''
	if 300 in rowsum or 300 in colsum or 300 in diagsums:
	    if not(300 in rowsum and 300 in colsum and 300 in diagsums):
	    	win += 'X'
	if 30 in rowsum or 30 in colsum or 30 in diagsums:
		if not(30 in rowsum and 30 in colsum and 30 in diagsums):
			win += 'O'
	return win

def isValid(tictac):
	xcount = 0
	ocount = 0
	integer_tictac = []
	for row in tictac:
		temp = []
		for move in row:
			if move == 'X':
				xcount += 1
				temp.append(100)
			elif move == 'O':
				ocount += 1
				temp.append(10)
			else:
				temp.append(0)
		integer_tictac.append(temp)
	if xcount == ocount or xcount == ocount+1:
		if ocount < 3:
			#at least 3 O's needed to win
			return True
		else:
			winner = checkwinning(integer_tictac)
			if winner == 'X':
				if (xcount == ocount+1):
					return False
			elif winner == 'O' :
				return True
			else:
				return False
	else:
		return False

if __name__ == "__main__":
	tictac = [['X', 'X', 'O'], 
              ['O', 'O', 'X'],
              ['X', 'O', 'X']]
	if isValid(tictac):
		print "Valid"
	else:
		print "Invalid"