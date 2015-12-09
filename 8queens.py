#/usr/bin/env python3.4
from itertools import permutations
n = 8
cols = range(n)
for vec in permutations(cols):
    S1 = set([vec[i] + i for i in cols])
    S2 = set([vec[i] - i for i in cols])
    if n == len(S1) == len(S2):
        print(vec)