#!/usr/local/bin/ python3

# str type name variable example
name = "Selin"

# int type age variable example
age = 21

def name_and_birth_year(name, age):
		# int type birth_year variable
		birth_year = int(2022) - int(age)

		# print name and birth_year
		print(name + " was born in " + str(birth_year))

		# return the name and year which a person was born
		return name + " was born in " + str(birth_year)


if __name__ == '__main__':
	# call the function
	name_and_birth_year('Mary', 28)
