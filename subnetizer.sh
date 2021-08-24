#!/usr/bin/env bash

cat addreses.txt | while read ip; do
  IFS=. read ip1 ip2 ip3 ip4 <<< "$ip"
  echo $ip1\.$ip2 >> addreses2.txt
done

cat addreses2.txt | sort | uniq
