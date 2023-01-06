#!/usr/local/bin/python3
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-
 
# create a function called round_str, which behaves identically to the built-in function round, if it receives a string converts the str to float and then rounds it to the specified number of decimal places, if it receives a float, it rounds it to the specified number of decimal places.
def round_str(value, num_decimal_places):

	if type(value) == str:
		value = float(value)
		return round(value, num_decimal_places)

	if type(value) == float:
		return round(value, num_decimal_places)


if __name__ == '__main__':
	# call the function
	print(round_str('3.141592653589793', 2))
	print(round_str(3.141592653589793, 4))
