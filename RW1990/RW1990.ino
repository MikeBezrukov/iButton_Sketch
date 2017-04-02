/*The sketch is for reading RW1990, Rw2004 compatible iButton; writing RW1990, RW1990.2 only.*/
/*Connect iButton to the pin with number PIN*/

#include <OneWire.h>
#define PIN 10
#define WRITE      /*Uncomment if need to write a key.*/
//#define RW1990_2   /*Uncomment if you deal with RW1990.2*/

//byte key_to_write[] = { 0x01, 0xE4, 0x6D, 0x7B, 0x00, 0x00, 0x00, 0x09 };  /*Home01*/
byte key_to_write[] = { 0x01, 0x00, 0x3E, 0x5C, 0x03, 0x00, 0x00, 0x35 };  /*Home02*/
//byte key_to_write[] = { 0x01, 0x5F, 0x0C, 0x84, 0x00, 0x00, 0x00, 0xC1 };  /*Home03*/

//byte key_to_write[]= { 0x01, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x9B };/*Universal for KS-2002,Metakom,Cyfral,Visit*/
//byte key_to_write[]= { 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x9B };/*~-_*/
//byte key_to_write[]= { 0x01, 0x00, 0x36, 0x5A, 0x11, 0x40, 0xBE, 0xE1 };/*Universal for KS-2002,Metakom,Visit*/
//byte key_to_write[]= { 0x01, 0xBE, 0x40, 0x11, 0x5A, 0x36, 0x00, 0xE1 };/*~-_Universal key for VIZIT*/
//byte key_to_write[]= { 0x01, 0x00, 0x00, 0x0A, 0x11, 0x40, 0xBE, 0x1D };/*Universal key for Cyfral*/

OneWire  ds(PIN);  // pin 10 is 1-Wire interface pin now

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  byte i;
  byte data[8];

  delay(2000);
  if (ds.reset() !=1) 
  {
    Serial.print("Waiting for iButton to be connected.\n");
    delay(3000);
    return;
  }

  
  printStatus("Reading iButton Key.");
  readKey(data);    
  Serial.print("CRC:\t");  Serial.println(OneWire::crc8(data, 7), HEX);
  
  if (data[0] & data[1] & data[2] & data[3] & data[4] & data[5] & data[6] & data[7] == 0xFF)
  {
    printStatus("iButton is clear!"); 
    return;
  }

  #ifndef WRITE 
  return;
  #endif
  
  // Check wether the key has been written
  for (i = 0; i < 8; i++)
    if (data[i] != key_to_write[i])  break;  
    else
      if (i == 7){
        printStatus("...iButton is already written!");
        return;
      }

  // Froce writing
  Serial.print("\nWritting new iButton Key:");
  if (! writeKey(key_to_write))  printStatus("iButton writing has been done!");
  else  printStatus("Failed to write the iButton!");
  
}
//END of main loop

#ifndef RW1990_2
void writeByte(byte data)
{
  for(int data_bit=0; data_bit<8; data_bit++){
    if (data & 1){
      digitalWrite(PIN, LOW);  pinMode(PIN, OUTPUT);  delayMicroseconds(60);     
      pinMode(PIN, INPUT); digitalWrite(PIN, HIGH);
      delay(10);
    } else {
      digitalWrite(PIN, LOW); pinMode(PIN, OUTPUT);  //delayMicroseconds(5);
      pinMode(PIN, INPUT); digitalWrite(PIN, HIGH);
      delay(10);
    }
    data = data >> 1;
  }
}

#else
//Use for RW1990.2
void writeByte(byte data)
{
  for(int data_bit=0; data_bit<8; data_bit++){
    if (data & 1){
      digitalWrite(PIN, LOW); pinMode(PIN, OUTPUT);
      pinMode(PIN, INPUT); digitalWrite(PIN, HIGH);
      delay(10);
    } else {
      digitalWrite(PIN, LOW); pinMode(PIN, OUTPUT);
      delayMicroseconds(40);
      pinMode(PIN, INPUT); digitalWrite(PIN, HIGH);
      delay(10);
    }
    data = data >> 1;
  }
}
#endif

void readKey(byte *data){
//ds.skip();  
  ds.reset();
delay(50);
  ds.write(0x33);  //RD cmd
  ds.read_bytes(data, 8);
  Serial.print("Key: \t");
  
    for(byte i = 0; i < 8; i++) {
      Serial.print(data[i], HEX);
      if (i != 7) Serial.print(":");
    }
  Serial.println();
}

byte writeKey(byte *data)
{
  ds.reset();  ds.write(0xD1);
  //send logical 0
  digitalWrite(PIN, LOW); pinMode(PIN, OUTPUT); delayMicroseconds(60);
  pinMode(PIN, INPUT); digitalWrite(PIN, HIGH); delay(10);
  
  ds.reset();  ds.write(0xD5);
  for(byte i=0; i<8; i++)
  {
    writeByte(key_to_write[i]);
    Serial.print('*');
  }
  
  ds.reset();  ds.write(0xD1);
  //send logical 1
  digitalWrite(PIN, LOW); pinMode(PIN, OUTPUT); delayMicroseconds(10);
  pinMode(PIN, INPUT); digitalWrite(PIN, HIGH); delay(10);
 
  //Validate write operation.
  byte data_r[8];
  ds.skip();  ds.reset();  ds.write(0x33);  //RD cmd
  ds.read_bytes(data_r, 8);
  
  for (byte i = 0; i < 8; i++)
    if (data_r[i] != data[i])  return -1;
    
  return 0;
}

void printStatus(char *state)
{
  Serial.print("\nStatus:\t"); Serial.println(state);
}

/*
Information Sources:
1) http://arduino.ru/forum/programmirovanie/onewire-zapis-na-bolvanku-rw1990
2) http://blog.danman.eu/cloning-ibutton-using-rw1990-and-avr/
3) http://sun-student.ru/hard/rw1990/finale.html
4) http://electromost.com/news/protokol_dlja_ehlektronnykh_kljuchej_rw1990/2011-04-24-35
*/


