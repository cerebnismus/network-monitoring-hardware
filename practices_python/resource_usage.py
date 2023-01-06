#!/usr/local/bin/python3
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-

# (RUM) - A simple resource usage monitor
# CPU, RAM, HDD, NETWORK and other resources can be monitored.

import time, psutil

''' 
total: total physical memory available.
available: the memory that can be given instantly to processes without the system going into swap. 
This is calculated by summing different memory values depending on the platform 
and it is supposed to be used to monitor actual memory usage in a cross platform fashion.
percent: the percentage usage calculated as (total - available) / total * 100 
'''

if __name__ == "__main__":
    
    cpu_usage = psutil.cpu_percent(interval=1)
    print("CPU Usage: " + str(cpu_usage) + "%")

    ram_usage = psutil.virtual_memory().percent
    print("RAM Usage: " + str(ram_usage) + "%")

    hdd_usage = psutil.disk_usage('/').percent
    print("HDD Usage: " + str(hdd_usage) + "%")

    network_sent = psutil.net_io_counters().bytes_sent
    network_recv = psutil.net_io_counters().bytes_recv
    network_usage = network_sent + network_recv
    network_usage = round(network_usage/1024/1024, 2)
    print("Network Usage: " + str(network_usage) + " MB")
    