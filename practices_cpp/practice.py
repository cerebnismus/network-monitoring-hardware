#!/usr/local/bin/python3
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-
 
 # output is like:
		
	#	1
	#	2 4
	#	3 6 9
	#	4 8 12 16
	#	5 10 15 20 25
	#	6 12 18 24 30 36
	#	7 14 21 28 35 42 49
 
def main():
	for i in range(1, 8):
		for j in range(1, i+1):
			print(j*i, end=' ')
		print()
  
if __name__ == '__main__':
	main()