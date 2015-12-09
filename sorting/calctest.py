from decimal import *

def sigmoid(X):
    '''Compute the sigmoid function '''
    #d = zeros(shape=(X.shape))
	getcontext().prec = 3
	e = Decimal(1).exp()
    den = 1.0 + e ** (-1.0 * X)
    d = 1.0 / den
    return d

if __name__ == '__main__':
	sigmoid(1.5)