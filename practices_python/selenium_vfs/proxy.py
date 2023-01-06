#!/usr/bin/ python
# -*- coding: utf-8 -*-
# github.com/cerebnismus

import random, time, os
from selenium import webdriver


def get_driver():
    options = webdriver.ChromeOptions()
    options.add_argument('--ignore-certificate-errors')
    options.add_argument('--ignore-ssl-errors')

    options.add_argument('--headless')
    options.add_argument('--no-sandbox')
    options.add_argument('--disable-dev-shm-usage')
    options.add_argument('--disable-gpu')
    options.add_argument('--disable-extensions')
    options.add_argument('--disable-application-cache')
    options.add_argument('--disable-infobars')
    options.add_argument('--disable-blink-features=AutomationControlled')
    options.add_argument('--disable-features=VizDisplayCompositor')
    
    # options.add_argument('--start-maximized')
    # options.add_argument('--start-fullscreen')
    options.add_argument('--single-process')
    options.add_argument('--incognito')

    options.add_experimental_option('useAutomationExtension', False)
    options.add_experimental_option('excludeSwitches', ['enable-automation'])

    options.add_argument('user-agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.116 Safari/537.36')

