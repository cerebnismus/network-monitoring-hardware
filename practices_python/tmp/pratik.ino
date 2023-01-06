// this is a practice file for the arduino course
// this file is for the arduino uno

// add the main libraries for the arduino
#include <Arduino.h>


// algoritma 

// sozde kod ( pseudo code )

// integer degiskenini, 
// setup fonksiyonunda tanimla
// loop fonksiyonunda kullan

// veri tipleri 
// int: sayi, rakam
// float: ondalik sayi derece gibi "23.54" gibi 
// char: harf "a" gibi (array (dizi mantigi))
// string: kelime "arduino" gibi 
// bool: true false (dusuk maliyetle kod yazmak icin elzem !)

int RolePini = 2;
bool RoleDurumu;

// int gecikme = 3000;

/*
milisecond2second(char sn){
	sn = sn * 1000;
	return sn;
}
*/

// led acik mi ?
// aciksa: true
// kapali:	false

// led kapanli mi ?
// kapanli: true
// acik: false

// void tiipnde bir fonksiyon deger dondurmez !


void setup() {
  // Role pini'ni cikis olarak ayarla
  pinMode(RolePini, OUTPUT);
  // seri portu acik
  Serial.begin(9600);
	
	// indentation (indentasyon / girinti) (space/tab)
  if (digitalRead(RolePini) == HIGH){
	RoleDurumu = true;
	Serial.println("Role Pinin Durumu: " + String(RoleDurumu));
}

void loop() {

  // acik durum
  digitalWrite(RolePini, LOW);
  delay(3000);
  
  // kapali durum
  digitalWrite(RolePini, HIGH);
  delay(3000);

  }
	
	// if kosulu (condition)
  if (digitalRead(RolePini) == LOW){

	RoleDurumu = false; // role durumunu ayarladik
	Serial.println("Role Pinin Durumu: " + String(RoleDurumu)); // role durumunu stringe donusturup yazdirdik

  }

}