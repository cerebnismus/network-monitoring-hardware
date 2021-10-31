#!/bin/bash

rm -rf TEMP_IP IP

sudo tcpdump -nn -q ip -l -c 222 and udp | awk '{print $3; fflush(stdout)}' > TEMP_IP

if ! pgrep -x '$!' > /dev/null
then
  cat TEMP_IP | cut -d ' ' -f 3 | cut -d '.' -f 1-4 | sort | uniq | grep -v '192.168.' | grep -v '8.8.8.8' > IP
  str=$(cat IP)
  curl 'https://api.ipgeolocation.io/ipgeo?apiKey=4f6f8fe3c5ac4162950d221fa24dfd58&ip='$str'&include=security,liveHostname,useragent' | python -m json.tool
fi
