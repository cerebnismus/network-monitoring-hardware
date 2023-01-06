#!/usr/local/bin/python3
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-
 
# Define a function called count_zeros which has one parameter "number".
# The function returns the number of zeros in the value passed in as argument "number".
# The argument "number" can be of type str, float or int.

# Therefor, the following function calls should return the following values:
# count_zeros(105) -> 1
# count_zeros("9070") -> 2
# count_zeros(0.0012) -> 3

# NOTE: the output of the function should be an integer.

def count_zeros(number):
		number = str(number)
		count = 0
		for i in number:
				if i == "0":
						count += 1
		return count

print(count_zeros(105))
print(count_zeros("9070"))
print(count_zeros(0.0012))