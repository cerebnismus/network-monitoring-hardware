# 2021
# oguzhan.ince@protonmail.com

#!/bin/bash

total=0
reach=0
unreach=0

## SUBNETIZER ##
cat ip.txt | while read ip; do
  IFS=. read ip1 ip2 ip3 ip4 <<< "$ip"
  echo $ip1\.$ip2 >> subnetizer.txt
done

sub=$(cat subnetizer.txt | sort | uniq)

## PING ##
cat ip.txt | while read ip; do
    ping -c1 -t1 -W5 "$ip" >/dev/null
    if [ $? -eq 0 ]; then
	echo "UP: $ip" 
    	echo "$ip" >> up-output.txt
    else
    	echo "DOWN: $ip" 
    	echo "$ip" >> down-output.txt
    fi
done

export U="up-output.txt"
export D="down-output.txt"

reach=$(wc -l "$U" 2>/dev/null | awk '{print $1}')
unreach=$(wc -l "$D" 2>/dev/null | awk '{print $1}')

total=$((reach + unreach))
perc=$((reach*100/total))

## OUTPUT ##
echo ""
echo "Block     : $sub "
echo "Total     : $total"
echo "Total UP  : $reach"
echo "Total DW  : $unreach"
echo "Reachable : %$perc"
echo ""

rm subnetizer.txt up-output.txt down-output.txt &>/dev/null
