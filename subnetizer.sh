#!/bin/bash

## SUBNETIZER ##
cat ip.txt | while read ip; do
  IFS=. read ip1 ip2 ip3 ip4 <<< "$ip"
  echo $ip1\.$ip2 >> subnetizer.txt
done

sub=$(cat subnetizer.txt | sort | uniq)

echo "Block     : $sub "
