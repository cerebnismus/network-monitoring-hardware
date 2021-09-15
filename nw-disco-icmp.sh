#!/bin/bash

# get connection info from network eq.
# (tryng fastest ping)

## VARIABLES ##
start="date"
bold=$(tput bold)
normal=$(tput sgr0)

total=0
reach=0
unreach=0

## PROMPT ##
echo -e "\nFILES:"
ls | grep .txt
echo -e -n "\nSpecify IP file: "
read ans

## PING ##
cat ip.txt | while read ip; do
    ping -c1 -t50 -W5 "$ip" >/dev/null
    if [ $? -eq 0 ]; then
	echo "OK: $ip" 
    	echo "$ip" >> icmp-ok-output.txt
    else
    	echo "${bold}NOK: $ip${normal}" 
    	echo "$ip" >> icmp-nok-output.txt
    fi
done

export OK="icmp-ok-output.txt"
export NOK="icmp-nok-output.txt"

reach=$(wc -l "$OK" 2>/dev/null | awk '{print $1}')
unreach=$(wc -l "$NOK" 2>/dev/null | awk '{print $1}')

total=$((reach + unreach))
perc=$((reach*100/total))

## SUBNETIZER OK ##
cat icmp-ok-output.txt | while read ip; do
  IFS=. read ip1 ip2 ip3 ip4 <<< "$ip"
  echo -e "$ip1\.$ip2\n" >> icmp-ok-subnetizer.txt
done

oks=$(cat icmp-ok-output.txt | sort | uniq)
echo -e "\nreachable blocks are:\n"
echo $oks

## SUBNETIZER NOK ##
cat icmp-nok-output.txt | while read ip; do
  IFS=. read ip1 ip2 ip3 ip4 <<< "$ip"
  echo -e "$ip1\.$ip2\n" >> icmp-nok-subnetizer.txt
done

noks=$(cat icmp-nok-subnetizer.txt | sort | uniq)
echo -e "\nunreachable blocks are:\n"
echo $noks

## OUTPUT ##
echo -e "\nTotal     : $total"
echo "Total OK  : $reach"
echo "Total NOK : $unreach"
echo "Reachable : %$perc"

end="date"
runtime=$((end-start))
echo -e "Runtime   : $runtime\n"

# rm icmp-ok-output.txt icmp-nok-output.txt icmp-ok-subnetizer.txt icmp-nok-subnetizer.txt &>/dev/null
