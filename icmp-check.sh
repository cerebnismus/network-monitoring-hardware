#!/usr/bin/env bash

for i in 'cat addr.txt'; do
    if ping -c 1 $i &> /dev/null
    then echo "$i" >> icmp-ok  # ICMP OK
    else echo "$i" >> icmp-nok # ICMP NOK
    fi
done
