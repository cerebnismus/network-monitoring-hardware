#!/usr/local/bin/python3
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-
 
# Define a function called replace_char_in_str which has three parameters original_str,pos and char.
# The function "replaces" a character in the string passed in as the argument original_str.
# Moreq precisely, the function replaces the character at the position specified by the integer pos with the character char.

# Therefore, the following function calls should produce the following outputs:

# 1. replace_char_in_str("zog", 0, "d") -> "dog"
# 2. replace_char_in_str("cet", 1, "a") -> "cat"
# 3. replace_char_in_str("Pyfhon", 2, "t") -> "Python"

# NOTE: notice that the positions of the characters in the string start with 0 instead of 1. This is because in computer programming there is a convetion of counting from 0 instead of 1.

def replace_char_in_str(original_str, pos, char):
		original_str = list(original_str)
		original_str[pos] = char
		return "".join(original_str)

print(replace_char_in_str("zog", 0, "d"))
print(replace_char_in_str("cet", 1, "a"))
print(replace_char_in_str("Pyfhon", 2, "t"))