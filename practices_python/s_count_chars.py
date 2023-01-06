#!/usr/local/bin/python3
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-
 
# Define a function called character_count which expects a string as an argument and returns a 
# dictionary whose keys are characters in the string, while the corresponding values are the
# number of times that character appears in the string.

# character_count("hello") -> {'h': 1, 'e': 1, 'l': 2, 'o': 1}

def character_count(string):
		dict = {}
		for i in string:
				if i in dict:
						dict[i] += 1
				else:
						dict[i] = 1
		return dict

print(character_count("hello"))