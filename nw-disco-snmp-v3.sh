#!/bin/bash

# get system data from network eq.
# (tryng snmpv3)

## VARIABLES ##
start=$(date +%s.%N)

bold=$(tput bold)
normal=$(tput sgr0)

total=0
reach=0
unreach=0

sha="1234"
aes="0000"

## PROMPT ##
echo -e "\nFILES:"
ls | grep .txt
echo -e -n "\nSpecify IP file: "
read ans

## PING ##
cat $ans | while read ip; do
    snmpwalk -v3 -t10 -r1 -l authPriv -u snmp-poller -a SHA -A "$sha" -x AES -X "$aes" "$ip" system >/dev/null
    if [ $? -eq 0 ]; then
	echo "OK: $ip" 
    	echo "$ip" >> snmpv3-ok-output.txt
    else
    	echo "${bold}NOK: $ip${normal}" 
    	echo "$ip" >> snmpv3-nok-output.txt
    fi
done

export OK="snmpv3-ok-output.txt"
export NOK="snmpv3-nok-output.txt"

reach=$(wc -l "$OK" 2>/dev/null | awk '{print $1}')
unreach=$(wc -l "$NOK" 2>/dev/null | awk '{print $1}')

total=$((reach + unreach))
perc=$((reach*100/total))

## SUBNETIZER OK ##
cat snmpv3-ok-output.txt | while read ip; do
  IFS=. read ip1 ip2 ip3 ip4 <<< "$ip"
  echo -e "$ip1\.$ip2\n" >> snmpv3-ok-subnetizer.txt
done

oks=$(cat snmpv3-ok-output.txt | sort | uniq)
echo -e "\nreachable blocks are:\n"
echo $oks

## SUBNETIZER NOK ##
cat snmpv3-nok-output.txt | while read ip; do
  IFS=. read ip1 ip2 ip3 ip4 <<< "$ip"
  echo -e "$ip1\.$ip2\n" >> snmpv3-nok-subnetizer.txt
done

noks=$(cat snmpv3-nok-subnetizer.txt | sort | uniq)
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

# rm snmpv3-ok-output.txt snmpv3-nok-output.txt snmpv3-ok-subnetizer.txt snmpv3-nok-subnetizer.txt &>/dev/null
