#!/bin/bash

du -sh /var/cache/apt/archives
sudo apt-get clean
sudo apt autoremove
sudo apt autoremove --purge
