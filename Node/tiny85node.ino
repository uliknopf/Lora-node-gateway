//tiny85node.ino
//Attiny85 sendet mit RFM95 Temperatur in konstanten Zeitabständen.
//ATtiny85 überwacht den Taster und sendet Nachricht bei Benutzung.
//

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <RFM95_S.h>
#include <Arduino.h>
#include <tinySPI.h>


unsigned long cycle=1;//zählt die Zyklen

int klingel = 3;//Arduino Pin 3 = physikalischer Pin 2
int NSS=4;//Arduino Pin 4 = physikalischer Pin 3
int sync_word=0x12;//privates Netz
RFM95_S rfm(NSS);  // (NSS)
uint8_t Data[50];//Maximale Payloadlänge
uint8_t Data_Length = 0x05;// 5Byte Gesamtlänge



void setup()
{
  pinMode(klingel,INPUT_PULLUP);
  rfm.init();//RFM95 initialisieren , danach Frequenzbereich und Synchronisations-Byte setzen.
  rfm.setSyncWord(sync_word); //LoRaWan syncWord = 0x34 /privateNetze syncWord = 0x12
  rfm.setFrequencyPlan(0);//0 => 868.100 MHz/  1 => 868.300 MHz / 2 => 868.500 MHz	/3 ist 867.100 MHz
  //4 => 867.300 MHz/  5 => 867.500 MHz / 6 => 867.700 MHz / 7 => 867.900 MHz
  rfm.setSpreadingFactor(7);
  
  // setup of the WatchdogTimer
  cli();
  wdt_reset(); // reset watchdog timer
  MCUSR &= ~(1 << WDRF); // remove reset flag
  WDTCR = (1 << WDCE); // set WDCE, access prescaler
  WDTCR = 1 << WDP0 | 1 << WDP2; // set prescaler bits to to 0.5s
  //WDP1=1,WDP2=1 --->1Sek  /WDP0=1,WDP2=1 --->0.5Sek /WDP3=1--->4Sek  /WDP0=1,WDP1=1,WDP2=1 --->2Sek
  WDTCR |= (1 << WDIE); // access WDT interrupt
  sei();//end WatchdogTimer setup
}

// Watchdog timer Interrupt Service Routine
ISR(WDT_vect)
{
  cycle++;
}

void loop()
{ 
    if (cycle%(117 *60 *1)==0 || digitalRead(klingel)==LOW) {// jede 1 h eine Messung
      createData();
      rfm.RFM_Send_Package(Data,Data_Length);
      delay(1000);
    }
	
	enterSleepMode();
  
}

void enterSleepMode()
{
    byte adcsra;

    adcsra = ADCSRA; // save ADC control and status register A
    ADCSRA &= ~(1 << ADEN); // disable ADC

    MCUCR |= (1 << SM1) & ~(1 << SM0); // Sleep-Modus = Power Down
    MCUCR |= (1 << SE); // set sleep enable
    sleep_cpu(); // sleep
    MCUCR &= ~(1 << SE); // reset sleep enable

    ADCSRA = adcsra; // restore ADC control and status register A
}

void createData()
{
  int klingeling=0;
  if (digitalRead(klingel)==LOW){klingeling=255;}
  uint16_t temp=readTemp();//Temperaturmessung
  Data[0]=1;
  Data[1]=2;
  Data[2]=3;
  Data[3]=klingeling;
  Data[4]=int(temp+100); // 20Grad C° sind dann 120 (keine negativen Werte) 
}

uint16_t readTemp() 
{ 
uint16_t result; // Read temperature sensor against 1.1V reference 
ADMUX = _BV(REFS1) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) |_BV(MUX0); //ATtiny85
//ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3);  //Arduino Uno 
//ADMUX = _BV(REFS1) | _BV(MUX5) | _BV(MUX1); //ATtiny84 interner Temperatur
delay(5); // Wait for Vref to settle 
ADCSRA |= _BV(ADSC); // Convert 
while (bit_is_set(ADCSRA,ADSC)); 
result = ADCL; 
result |= ADCH<<8; 
int drift=270;//Relative Abweichung vom Ist-Temperaturwert
result = result-drift;   // Result in Grad Celsius
return result;
}        //Kalibrierung ist notwendig
