#!/usr/local/bin/python3
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-

# Define a function called avg which expects a list of numbers as an argument and returns the average.
# The output should be rounded to two decimal places. Without round function.
# Below are some of the values this function should return:

# 1. avg([1, 2, 3]) -> 2.0
# 2. avg([-4.3, 2.31, 18.74]) -> 5.58

def avg(numbers):
		sum = 0
		for i in numbers:
				sum += i

		sum = sum / len(numbers)
		sum = sum * 100
		sum = int(sum)
		return sum / 100

print(avg([1, 2, 3]))
print(avg([-4.3, 2.31, 18.74]))
