#!/usr/local/bin/ python3

# Define a function called sum_odd which has two parameters "start" and "stop".
# The function calculates the sum of all odd numbers in the range specified by arguments start and stop.
# The integer passed in as argument start should be included in the range, while the integer stop should be excluded from the range.

# Therefor, the following function calls should return the following values:
# sum_odd(0, 10) -> 25
# sum_odd(24, 68) -> 1012
# sum_odd(4, 5) -> 0

def sum_odd(start, stop):
		sum = 0
		for i in range(start, stop):
				if i % 2 != 0:
						sum += i
		return sum

print(sum_odd(0, 10))
print(sum_odd(24, 68))
print(sum_odd(4, 5))
