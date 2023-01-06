#!/usr/local/bin/ python3

# Define a function called remove_duplicates which expects one list as an argument and returns a new list, which is a copy pf the original list with all the duplicate items removed.
#When removing items, the first occurence of an item should be kept,while all subsequent duplicates of that item are removed.

# NOTE: the function should not modify the original list receives as an argument.
# NOTE: Therefore, you must create a new list and return that list.

# someof the function calls and their expected outputs are shown below:
# remove_duplicates([1, 1, 1, 2, 1, 3, 2]) -> [1, 2, 3,]
# remove_duplicates(["cat", "dog", "dog", "cat"]) -> ["cat", "dog"]

def remove_duplicates(list):
		new_list = []
		for i in list:
				if i not in new_list:
						new_list.append(i)
		return new_list

print(remove_duplicates([1, 1, 1, 2, 1, 3, 2]))
print(remove_duplicates(["cat", "dog", "dog", "cat"]))
