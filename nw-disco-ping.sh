#!/bin/bash

cat ip.txt |  while read ip
do
    ping -c 1 "$ip" > /dev/null
    if [ $? -eq 0 ]; then
    echo "$ip" >> up-output.txt
    else
    echo "$ip" >> down-output.txt
    fi
done
