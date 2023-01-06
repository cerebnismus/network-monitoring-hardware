#!/bin/sh

# cerebnismus - eVize.sh
# eVize Script

ISIM="OGUZHAN"
SOYISIM="INCE"
ULKE_KODU="90"
TELEFON="5304543426"


curl -s --location-trusted -X GET "https://secure.e-konsulat.gov.pl/placowki/125/wiza-krajowa/wizyty/weryfikacja-obrazkowa" > first.txt


cat first.txt