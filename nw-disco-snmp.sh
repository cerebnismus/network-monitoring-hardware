#!/bin/bash

cat ip.txt |  while read ip
do
    snmpwalk -c public -v2c -t 10 -r 1 "$ip" system > /dev/null
    if [ $? -eq 0 ]; then
    #echo "SNMP DOWN: $ip" 
    echo "$ip" >> up-output-snmp.txt
    else
    #echo "SNMP UP: $ip" 
    echo "$ip" >> down-output-snmp.txt
    fi

    echo "Total SNMP UP: " && cat up-output-snmp.txt | wc -l 
    echo "Total SNMP DOWN: " && cat down-output-snmp.txt | wc -l 

done
