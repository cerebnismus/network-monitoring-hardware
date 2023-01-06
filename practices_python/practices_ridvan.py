#!/usr/local/bin/python3
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-

# 4.1.2023 14:50 

# Soru1
# A - json türünde bir dosya oluşturun. Bu json dosyasında 5 özellikli 2 adet daire bulunsun.
# Her dairenin mutlak olması gereken özelliği vardır. Bunlar daire_id, satılma_durumu.
# Json olusturduktan sonra daire_id = 1 olan dairenin satılma_durumu = 'Satıldı' olarak güncellenmeli.

def soru1a():
		import json
		daireler = [
				{
						"daire_id": 1,
						"oda_sayisi": 3,
						"kat": 4,
						"metrekare": 120,
						"satılma_durumu": "Satılmadı"
				},
				{
						"daire_id": 2,
						"oda_sayisi": 2,
						"kat": 2,
						"metrekare": 80,
						"satılma_durumu": "Satılmadı"
				}
		]

		''' Burasi odeve dahil degil sadece sonucu gorun diye ekledim '''
		print('\n\nDaireler: Ilk durum:')
		print(daireler)


		with open('daireler.json', 'w') as f:
				json.dump(daireler, f)

		with open('daireler.json', 'r') as f:
				daireler = json.load(f)

		for daire in daireler:
				if daire['daire_id'] == 1:
						daire['satılma_durumu'] = 'Satıldı'

		with open('daireler.json', 'w') as f:
				json.dump(daireler, f)
    
		''' Burasi odeve dahil degil sadece sonucu gorun diye ekledim '''
		print('\n\nDaireler: Son durum:')
		print(daireler)

# B - CREATE TABLE hocalar sicil_no CHAR(9) PRIMARY KEY, ad VARCHAR, soyad VARCHAR, unvani VARCHAR 
# ile oluşturulmuş bir sqlite tablosuna ders aldığınız hocanın bilgilerini ekleyin. Ardından
# unvanını 'Prof. Dr.' olarak güncelleyin. Try-except blokları kullanın. Transaction kullanın.

def soru1b():
		import sqlite3

		conn = sqlite3.connect('hocalar.db')
		c = conn.cursor()

		c.execute('''CREATE TABLE IF NOT EXISTS hocalar
				(sicil_no CHAR(9) PRIMARY KEY, ad VARCHAR, soyad VARCHAR, unvani VARCHAR)''')

		try:
				c.execute('''INSERT INTO hocalar VALUES ('123456789', 'Oguzhan', 'Ince', 'Dr.')''')
				conn.commit()
		except sqlite3.IntegrityError:
				print('Hata: Sicil numarası zaten var.')
  
		''' Burasi odeve dahil degil sadece sonucu gorun diye ekledim '''
		print('\n\nHocalar Ilk durum:')
		for row in c.execute('SELECT * FROM hocalar'):
				print(row)
  
		c.execute('''UPDATE hocalar SET unvani = 'Prof. Dr.' WHERE sicil_no = '123456789' ''')
		conn.commit()
  
		''' Burasi odeve dahil degil sadece sonucu gorun diye ekledim '''
		print('\n\nHocalar Son durum:')
		for row in c.execute('SELECT * FROM hocalar'):
				print(row)

		conn.close()
  
soru1a()
soru1b()


# Soru2
# 1986 - 1995 arası yıllardaki her gün ve o günün sıcaklık değerleri bir pandas dataframe'de bulunmakta.
# Tüm yıllardaki her günün sıcaklık değerleri toplu olarak; veri = sicakliklar['degerler'].values şeklinde tutulmaktadır.
# hazır geldi varsayılan artikYilKontrol(yil) fonksiyonu ile artık yılların bulunması sağlanmıştır. True dönerse artık yıldır.
# Buna göre, verilen yıllar arasındaki sıcaklıkları ayrı ayrı yazdıran bir fonksiyon yazınız.

''' Burasi odeve dahil degil sadece sonucu gorun diye ekledim  '''
def generate_sicakliklar_csv():
		import pandas as pd
		import numpy as np

		veri = np.random.randint(0, 40, 365 * 10)
		veri = pd.DataFrame(veri, columns = ['degerler'])

		veri.to_csv('sicakliklar.csv', sep = ';')
	


import pandas as pd

sicakliklar = pd.read_csv('sicakliklar.csv', sep = ';')
veri = sicakliklar['degerler'].values


''' Burasi odeve dahil degil sadece sonucu gorun diye ekledim '''
def artikYilKontrol(yil):
		if yil % 4 == 0:
				if yil % 100 == 0:
						if yil % 400 == 0:
								return True
						else:
								return False
				else:
						return True
		else:
				return False


def sicaklikYazdir(baslangicYili, bitisYili): 
		for yil in range(baslangicYili, bitisYili + 1): 
				if artikYilKontrol(yil): 
						veri = veri[366:] # 366 - sona kadar
						print(veri[0:366]) # 0 - 365 arası
				else:
						veri = veri[365:] # 365 - sona kadar
						print(veri[0:365]) # 0 - 364 arası
