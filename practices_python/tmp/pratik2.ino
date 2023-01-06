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
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);
int Role0=4;
int Role1=5;
int Ses=6;


 


void setup() {

  Serial.begin(9600);
  Serial.println("Oda Nem ve Sıcaklık Koruma");
  delay(200);
  dht.begin();
  pinMode(Role0,OUTPUT);
  pinMode(Role1,OUTPUT);
  pinMode(Ses,OUTPUT);
  int i=1;
  
}

void loop() {

  float Temp = dht.readTemperature();
  int Hum = dht.readHumidity();
  Serial.println("Sicaklik: ");
  Serial.println(Temp);
  Serial.print("Nem:% ");
  Serial.println(Hum);

  if (Hum>60)
  {
    digitalWrite(Role0,HIGH);
  
  }
  



  if(Hum<=60){
    digitalWrite(Role0,LOW);
    i=0;
    }else
    digitalWrite(Role0,HIGH);
     if(Hum<=55 && i=0){
     i=1 
    digitalWrite(Ses,HIGH);
    delay(100);
    digitalWrite(Ses,LOW);
    delay(100);
      
    }
   

    
  if(Temp<=26.50){
      digitalWrite(Role1,LOW);
      
    }else
    digitalWrite(Role1,HIGH);
    
      
    
  delay(500);
  
  


}