#!/bin/bash

# get system data from network eq.
# (tryng two community strings)

bold=$(tput bold)
normal=$(tput sgr0)

com_str_1="public"
com_str_2="public2"

cat ip.txt | while read ip; do
    snmpwalk -c "$com_str_1" -v2c -t10 -r1 "$ip" system >/dev/null
    if [ $? -eq 0 ]; then
            echo "OK: $ip : $com_str_1" 
            echo "$ip" >> snmp-ok.txt
    else
        snmpwalk -c "$com_str_2" -v2c -t10 -r1 "$ip" system >/dev/null
        if [ $? -eq 0 ]; then
            echo "OK: $ip : $com_str_2" 
            echo "$ip : $com_str_2" >> snmp-ok.txt
        else
        echo "${bold}NOK: $ip ${normal}"  
        echo "$ip" >> snmp-nok.txt
        fi
    fi
done
