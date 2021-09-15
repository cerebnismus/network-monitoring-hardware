#!/bin/bash

# get system data from network eq.
# (tryng two community strings)

## VARIABLES ##
start=$(date +%s.%N)

bold=$(tput bold)
normal=$(tput sgr0)

total=0
reach=0
unreach=0

com_str_1="public"
com_str_2="public2"

## PROMPT ##
echo -e "\nFILES:"
ls | grep .txt
echo -e -n "\nSpecify IP file: "
read ans

cat $ans | while read ip; do
    snmpwalk -c "$com_str_1" -v2c -t10 -r1 "$ip" system >/dev/null
    if [ $? -eq 0 ]; then
            echo "OK: $ip : $com_str_1" 
            echo "$ip" >> snmp-ok-output.txt
    else
        snmpwalk -c "$com_str_2" -v2c -t10 -r1 "$ip" system >/dev/null
        if [ $? -eq 0 ]; then
            echo "OK: $ip : $com_str_2" 
            echo "$ip : $com_str_2" >> snmp-ok-output.txt
        else
        echo "${bold}NOK: $ip ${normal}"  
        echo "$ip" >> snmp-nok-output.txt
        fi
    fi
done

export OK="snmp-ok-output.txt"
export NOK="snmp-nok-output.txt"

reach=$(wc -l "$OK" 2>/dev/null | awk '{print $1}')
unreach=$(wc -l "$NOK" 2>/dev/null | awk '{print $1}')

total=$((reach + unreach))
perc=$((reach*100/total))

## SUBNETIZER OK ##
cat snmp-ok-output.txt | while read ip; do
  IFS=. read ip1 ip2 ip3 ip4 <<< "$ip"
  echo -e "$ip1\.$ip2\n" >> snmp-ok-subnetizer.txt
done

oks=$(cat snmp-ok-output.txt | sort | uniq)
echo -e "\nreachable blocks are:\n"
echo $oks

## SUBNETIZER NOK ##
cat snmp-nok-output.txt | while read ip; do
  IFS=. read ip1 ip2 ip3 ip4 <<< "$ip"
  echo -e "$ip1\.$ip2\n" >> snmp-nok-subnetizer.txt
done

noks=$(cat snmp-nok-subnetizer.txt | sort | uniq)
echo -e "\nunreachable blocks are:\n"
echo $noks

## OUTPUT ##
echo -e "\nTotal     : $total"
echo "Total OK  : $reach"
echo "Total NOK : $unreach"
echo "Reachable : %$perc"

## RUNTIME ##
duration=$(echo "$(date +%s.%N) - $start" | bc)
execution_time=`printf "%.2f seconds" $duration`
echo -e "Runtime   : $execution_time\n"

# rm snmp-ok-output.txt snmp-nok-output.txt snmp-ok-subnetizer.txt snmp-nok-subnetizer.txt &>/dev/null
