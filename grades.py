#!/usr/bin/env python3.4
""""""
# This program gives the aggregate grade of the student
""""""
i = 0
j = 0
weights = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
grades = [0, 0, 0, 0, 0, 0]
fname = input()
sum = 0.0
for x in open(fname, "r"):
    if i <= 5:
        weights[i] = float(x)
        i = i + 1
    else:
        grades[j] = int(x)
        j = j + 1

for x in range(0, 6):
    sum = sum + (weights[x] * grades[x])

if sum >= 93.5:
    print ("A")
elif 87.0 <= sum < 93.5:
    print ("B")
elif 80.0 <= sum < 87.5:
    print ("C")
elif 70.0 <= sum < 80.0:
    print ("D")
else:
    print ("F")