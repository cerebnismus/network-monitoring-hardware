#!/usr/local/bin/python3
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-
 
# create a function called total_price, which has two parameters num_apples and num_oranges, and returns the total price of the apples and oranges according to the following rules:
# 1. The price of an apple is 0.36 dollars
# 2. The price of an orange is 0.42 dollars
# 3. If a customer buys 3 or more apples, a %15 discount is applied to the total price of the apples
# 4. If a customer buys 4 or more oranges, a %10 discount is applied to the total price of the oranges
# 5. If a customer buys 10 or more items in total, an additional %25 discount is applied to the total price, after the previous discounts are applied
# 6. The total price is rounded to two decimal places

def total_price(num_apples, num_oranges):

	total_price = 0
	price_apple = 0.36
	price_orange = 0.42

	price_of_apples = num_apples * price_apple
	price_of_oranges = num_oranges * price_orange

	if num_apples >= 3:
		price_of_apples = price_of_apples - (price_of_apples * 0.15)

	if num_oranges >= 4:
		price_of_oranges = price_of_oranges - (price_of_oranges * 0.10)

	total_price = price_of_apples + price_of_oranges

	if (num_apples + num_oranges) >= 10:
		total_price = total_price - (total_price * 0.25)

	return round(total_price, 2)

# define a function called total_price_modified, which is a modified version of the function defined in the previous exercise
# More specifically, contition 5. is cganged so that if a customer buys 10 or more items, previous discounts (specified in conditions 3. and 4.) are not applied and the only discount applied is the 25% discount specified in condition 5.
# However, if a customer has less than 10 items, the previous discounts are applied as specified in conditions 3. and 4.
def total_price_modified(num_apples, num_oranges):

	total_price = 0
	price_apple = 0.36
	price_orange = 0.42

	price_of_apples = num_apples * price_apple
	price_of_oranges = num_oranges * price_orange

	if (num_apples + num_oranges) < 10:
		if num_apples >= 3:
			price_of_apples = price_of_apples - (price_of_apples * 0.15)

		if num_oranges >= 4:
			price_of_oranges = price_of_oranges - (price_of_oranges * 0.10)

	total_price = price_of_apples + price_of_oranges

	if (num_apples + num_oranges) >= 10:
		total_price = total_price - (total_price * 0.25)

	return round(total_price, 2)

if __name__ == '__main__':

	print(total_price(7, 5))
	print(total_price_modified(7, 5))