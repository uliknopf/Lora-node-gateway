/*
  RFM95.h - Library header file for RFM95 LoRa module.
  Created by Leo Korbee, March 31, 2018.
  Released into the public domain.
  @license Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
  Thanks to all the folks who contributed beforme me on this code.


//I changed the code to meet my needs: set the sync_word, set fixed frequency,
//set spreadingFactor inside the .ino file of the Arduino IDE and I do without
//the DIO0-pin to gain an extra pin on my ATtiny85; I renamed the library to RFM95_S.h 
//  Uli Knopf , June 2023.
//Fixed Bandwidth of 125 kHz, fixed CodingRate of 4/5 ,European Frequencies
//FrequencyPlan(0)-868.1 MHz/ FrequencyPlan(1)-868.3 MHz/ FrequencyPlan(2)-868.5 MHz/
//FrequencyPlan(3)-867.1 MHz/ FrequencyPlan(4)-867.3 MHz/ FrequencyPlan(5)-867.5 MHz/
//FrequencyPlan(6)-867.7 MHz/ FrequencyPlan(7)-867.9 MHz/
*/

#ifndef RFM95_S_h
#define RMF95_S_h

#include "Arduino.h"

class RFM95_S
{
  public:
    RFM95_S(int NSS);
    void init();
    void RFM_Write(unsigned char RFM_Address, unsigned char RFM_Data);
    unsigned char RFM_Read(unsigned char RFM_Address);
    void RFM_Send_Package(unsigned char *RFM_Tx_Package, unsigned char Package_Length);
	void setSyncWord(uint8_t sync_word);
	void setFrequencyPlan(uint8_t frequencyPlan);//number 0 - 7
	void setSpreadingFactor(uint8_t sf);
  private:
    int _NSS;
};


#endif
