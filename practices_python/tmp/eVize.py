#!/usr/local/bin/ python3
# -*- coding: utf-8 -*-

import requests

# D type country visa appointment automation (turkey to poland)

NAME = "OGUZHAN"
LASTNAME = "INCE"
COUNTRY_CODE = "90"
PHONE = "5304543426"

CATEGORY = "1- Long Term Visa"
SUB_CATEGORY = "2- TC (Turkish Citizenship) Application - Working Visa"
VISA_CENTRE = "Ankara"


# url = "https://evisa.gov.tr/evize/evize.php"
# url = "https://secure.e-konsulat.gov.pl/placowki/125/wiza-krajowa/wizyty/weryfikacja-obrazkowa"



response = requests.get(url)

# print the response text
print(response.text)
