#!/bin/bash

cat ip.txt |  while read ip
do
    ping -c1 -t1 -W5 "$ip" > /dev/null
    if [ $? -eq 0 ]; then
    #echo "DOWN: $ip" 
    echo "$ip" >> up-output.txt
    else
    #echo "UP: $ip" 
    echo "$ip" >> down-output.txt
    fi

    echo "Total UP: " && cat up-output.txt | wc -l 
    echo "Total DOWN: " && cat down-output.txt | wc -l 

done
