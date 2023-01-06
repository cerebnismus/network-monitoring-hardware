#!/bin/bash

## VARIABLES ##
start=$(date +%s)

## To sleep for 1 seconds ##
sleep 1

## RUNTIME ##
duration=$(echo "$(date +%s) - $start" | bc)
execution_time=$(printf "%.2f seconds" $duration)
echo -e "Runtime   : $execution_time\n"
