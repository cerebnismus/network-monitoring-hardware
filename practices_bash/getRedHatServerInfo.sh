#!/bin/bash
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-

# get RedHat server information and print to console
# 2018-01-30 useful for armada inventory entry 

printf ' \n —————————————————————————————'
printf ' \n | REDHAT SERVER INFORMATION |'
printf ' \n —————————————————————————————'

uptime1=$(uptime | cut -d, -f1)
printf '\n\n -*-%s' "$uptime1"

uname1=$(uname -nr)
printf '\n -*- %s' "$uname1"


ip1=$(ifconfig | grep 'inet ' | cut -d: -f2 | awk '{ print $2}')

i=0
printf '\n\n -*- Interfaces:'
for ip in $ip1; do
		let i+=1
		printf '\n     IP%s: %s' "$i" "$ip"
done

# sudo permission required for dmidecode command
# serial1=$(dmidecode -t system | grep 'VMware-')
# echo "$serial1"
 
lscpu1=$(lscpu | grep 'Vendor ID:')
printf '\n\n -*- %s' "$lscpu1"

lscpu2=$(lscpu | grep 'Model name:')
printf '\n -*- %s' "$lscpu2"

lscpu3=$(lscpu | grep 'Architecture')
printf '\n -*- %s' "$lscpu3"

lscpu4=$(lscpu | grep 'CPU MHz:')
printf '\n -*- %s' "$lscpu4"

lscpu5=$(lscpu | grep 'CPU(s):' | head -n 1)
printf '\n -*- %s' "$lscpu5"

lscpu6=$(lscpu | grep 'Core(s) per socket:')
printf '\n -*- %s' "$lscpu6"

lscpu7=$(lscpu | grep 'Socket(s):')
printf '\n -*- %s' "$lscpu7"

mem1=$(cat /proc/meminfo | grep MemTotal)
printf '\n -*- %s' "$mem1"

total_G=0
disks_app_G=$(df -BG | grep ' /app' | awk '{ print $2}' | grep 'G' | cut -dG -f1)
for g in $disks_app_G; do
		let total_G+=$g
done

disks_home_G=$(df -BG | grep ' /home' | awk '{ print $2}' | grep 'G' | cut -dG -f1)
for g in $disks_home_G; do
		let total_G+=$g
done

printf '\n -*- DiskTotal:       %sG\n\n' "$total_G"
