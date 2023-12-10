/*
Das Gateway beept kurz wenn: start-RFM95 ist initialisiert.
Das Gateway beept lang wenn: Nachricht empfangen.
Das Gateway empfängt Daten der Node.  


ESP32---------+------RFM95
--------------+------------
GND-----------+------GND
3V3-----------+------3.3V
interrupt D15-+------DIO0
SS pin D2-----+-----NSS
SCK pin D18---+-------SCK
MOSI pin D23--+--------MOSI
MISO pin D19--+--------MISO 

*/


#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 2;          // CS (ChipSelect) SS  (SlaveSelect) Pin 
const int resetPin = 17;       // LoRa radio reset
const int irqPin = 15;         // DIO0 Pin; 
const int speaker=16;         //Board
uint8_t paylArray[100];
//buzzer
int freq = 1000;
int channel = 0;
int resolution = 8;

void setup() {

  Serial.begin(9600);                   // initialize serial
  while (!Serial);
  Serial.println("LoRa Receiver ESP32");
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, DIO0 pin
  if (!LoRa.begin(868.1E6)) {             // initialize ratio at 868 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  LoRa.setSyncWord(0x12);
  LoRa.setSpreadingFactor(7);
  ledcAttachPin(speaker, channel);
  buzzerstart();
  
}


void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // read packet
    int indize=0;
    while (LoRa.available()) {
      paylArray[indize] = (uint8_t)LoRa.read();
      if (indize>50){break;} // Höchstlänge an Bytes (50 Bytes, nicht mehr)
      indize++;
    }
    int sendeLaenge=indize;
    if (checkPayload()) {
      buzzerempfang();//langes Signal
      for (int i=0;i<sendeLaenge;i++){
        Serial.print(paylArray[i]);
        Serial.print(", ");
      }
      Serial.println();
    }
  
  }
}

bool checkPayload(){
	//Individuelle Bytes als Schlüssel
	if (paylArray[0]==1 && paylArray[1]==2 && paylArray[2]==3){return true;}
	else {return false;}
}

void buzzerstart(){
  //akustische Rückmeldung; alles ok
  ledcWriteTone(channel, freq);
  delay(500);        // .5 sec Pause
  ledcWriteTone(channel, 0);
}

void buzzerempfang(){
  //akustische Rückmeldung; Nachricht empfangen
  ledcWriteTone(channel, freq);
  delay(2000);        // 2 sec Pause
  ledcWriteTone(channel, 0);
}
