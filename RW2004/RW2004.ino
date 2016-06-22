/*The sketch is for reading RW1990, Rw2004 compatible iButton; writing RW2004 only.*/
/*Connect iButton to the pin with number PIN*/

#include <OneWire.h>
#define PIN 10
#define WRITE  /*If need to write a key.*/

byte key_to_write[] = { 0x01, 0xE4, 0x6D, 0x7B, 0x00, 0x00, 0x00, 0x09 };  /*Home01*/
//byte key_to_write[] = { 0x01, 0x00, 0x3E, 0x5C, 0x03, 0x00, 0x00, 0x35 };  /*Home02*/

//byte key_to_write[]= { 0x01, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x9B };/*Universal for KS-2002,Metakom,Cyfral,Visit*/
//byte key_to_write[]= { 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x9B };/*~-_*/
//byte key_to_write[]= { 0x01, 0x00, 0x36, 0x5A, 0x11, 0x40, 0xBE, 0xE1 };/*Universal for KS-2002,Metakom,Visit*/
//byte key_to_write[]= { 0x01, 0xBE, 0x40, 0x11, 0x5A, 0x36, 0x00, 0xE1 };/*~-_Universal key for VIZIT*/
//byte key_to_write[]= { 0x01, 0x00, 0x00, 0x0A, 0x11, 0x40, 0xBE, 0x1D };/*Universal key for Cyfral*/

OneWire  ds(PIN);

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  byte i;
  byte data[8];

  delay(2000);
  printStatus("Start operation.");

  readKey(data);    
  Serial.print("CRC:\t");  Serial.println(OneWire::crc8(data, 7), HEX);
  
  if (data[0] & data[1] & data[2] & data[3] & data[4] & data[5] & data[6] & data[7] == 0xFF)
  {
    printStatus("...iButton is not found!"); 
    return;
  }

#ifndef WRITE 
  return;
#endif

  // Check if read key is match to the written one
  for (i = 0; i < 8; i++)
    if (data[i] != key_to_write[i])
      break;
    else
      if (i == 7){
        printStatus("...iButton is already written!");
        return;
      }

  // Froce writing
  Serial.print("\nWritting new iButton Key:");
  if (! writeKey())  printStatus("iButton writing has been done!");
  
}



void send_programming_impulse()
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, HIGH); 
  delay(60);
  digitalWrite(PIN, LOW); 
  delay(5);
  digitalWrite(PIN, HIGH); 
  delay(50); 
}


void readKey(byte *data){
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


byte writeKey(void)
{
  byte data[4];
  
  for (uint8_t i = 0; i < 8; i++)
  {
    ds.reset();
    data[0] = 0x3C; // WR cmd
    data[1] = i;
    data[2] = 0;
    data[3] = key_to_write[i];
    ds.write_bytes(data, 4);
    Serial.print('*');
  
    uint8_t crc = ds.read();
    
    if (OneWire::crc8(data, 4) != crc) 
    {
        Serial.print("CRC error!\r\n");
        return -1;
    }
    else
        Serial.print('*');
    
    send_programming_impulse();
  }
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
