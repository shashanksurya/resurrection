#!/usr/bin/env python3.4
"""This program is for testing the longest palindrome"""


def pal(word):
    word = word.lower()
    results = []
    for i in range(len(word)):
        for j in range(0, i):
            bit = word[j:i + 1]
            if bit == bit[::-1]:
                results.append(bit)
    return word.index(max(results, key=len)), results


if __name__ == "__main__":
    i = input()
    List = pal(i)
    print(max(List[1], key=len))
