#!/bin/bash

# get system data from network eq.
# (tryng two community strings)

bold=$(tput bold)
normal=$(tput sgr0)

sha="1234"
aes="0000"

cat ip.txt | while read ip; do
    snmpwalk -v3 -t10 -r1 -l authPriv -u snmp-poller -a SHA -A "$sha" -x AES -X "$aes" "$ip" system >/dev/null
    if [ $? -eq 0 ]; then
        echo "OK: $ip" 
        echo "$ip" >> snmp-ok.txt
    else
        echo "${bold}NOK: $ip ${normal}"  
        echo "$ip" >> snmp-nok.txt
    fi
done
