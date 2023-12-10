/*
  RFM95.cpp - Library for RFM95 LoRa module.
  Created by Leo Korbee, March 31, 2018.
  Released into the public domain.
  @license Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
  Thanks to all the folks who contributed on the base of this code.
  (Gerben den Hartog, et al - Ideetron.nl)

//I changed the code to meet my needs: set the sync_word, set fixed frequency,
//set spreadingFactor inside the .ino file of the Arduino IDE and I do without
//the DIO0-pin to gain an extra pin on my ATtiny85; I renamed the library to RFM95_S.h 
//  Uli Knopf , June 2023.
*/
#include "Arduino.h"
#include "RFM95_S.h"
#include <tinySPI.h>

// constructor
RFM95_S::RFM95_S(int NSS)
{
  //_DIO0 = DIO0;no need because just sending with RFM95
  _NSS = NSS;
  // init tinySPI
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();
}


/*
*****************************************************************************************
* Description: Function used to initialize the RFM module on startup
*****************************************************************************************
*/
void RFM95_S::init()
{
  // set pinmodes input/output
  pinMode(_NSS, OUTPUT);
  //pinMode(_DIO0, INPUT);

  // NSS for starting and stopping communication with the RFM95 module
  digitalWrite(_NSS, HIGH);

  //Switch RFM to sleep
  RFM_Write(0x01,0x00);

  //Set RFM in LoRa mode
  RFM_Write(0x01,0x80);

  //Set RFM in Standby mode wait on mode ready
  RFM_Write(0x01,0x81);
  /*
  while (digitalRead(DIO5) == LOW)
  {
  }
  */
  delay(10);

  //Set carrair frequency
  // 868.100 MHz / 61.035 Hz = 14222987 = 0xD9068B
  RFM_Write(0x06,0xD9);
  RFM_Write(0x07,0x06);
  RFM_Write(0x08,0x8B);

  //PA pin (maximal power)
  RFM_Write(0x09,0xFF);

  //BW = 125 kHz, Coding rate 4/5, Explicit header mode
  RFM_Write(0x1D,0x72);

  //Spreading factor 7, PayloadCRC On
  RFM_Write(0x1E,0x74);

  //Rx Timeout set to 37 symbols
  RFM_Write(0x1F,0x25);

  //Preamble length set to 8 symbols
  //0x0008 + 4 = 12
  RFM_Write(0x20,0x00);
  RFM_Write(0x21,0x08);

  //Low datarate optimization off AGC auto on 
  //RFM_Write(0x26,0x04); //Low datarate optimization off AGC auto on
  RFM_Write(0x26,0x04);//Uli original: RFM_Write(0x26,0x0C);

  //Set LoRa sync word
  RFM_Write(0x39,0x12);

  //Set IQ to normal values
  RFM_Write(0x33,0x27);
  RFM_Write(0x3B,0x1D);

  //Set FIFO pointers
  //TX base adress
  RFM_Write(0x0E,0x80);
  //Rx base adress
  RFM_Write(0x0F,0x00);

  //Switch RFM to sleep
  RFM_Write(0x01,0x00);
}


/*
*****************************************************************************************
* Description : Funtion that writes a register from the RFM
*
* Arguments   : RFM_Address Address of register to be written
*         RFM_Data    Data to be written
*****************************************************************************************
*/

void RFM95_S::RFM_Write(unsigned char RFM_Address, unsigned char RFM_Data)
{
  //Set NSS pin Low to start communication
  digitalWrite(_NSS,LOW);

  //Send Addres with MSB 1 to make it a write command
  SPI.transfer(RFM_Address | 0x80);
  //Send Data
  SPI.transfer(RFM_Data);

  //Set NSS pin High to end communication
  digitalWrite(_NSS,HIGH);
}

/*
*****************************************************************************************
* Description : Funtion that reads a register from the RFM and returns the value
*
* Arguments   : RFM_Address Address of register to be read
*
* Returns   : Value of the register
*****************************************************************************************
*/

unsigned char RFM95_S::RFM_Read(unsigned char RFM_Address)
{
  unsigned char RFM_Data;

  //Set NSS pin low to start SPI communication
  digitalWrite(_NSS,LOW);

  //Send Address
  SPI.transfer(RFM_Address);
  //Send 0x00 to be able to receive the answer from the RFM
  RFM_Data = SPI.transfer(0x00);

  //Set NSS high to end communication
  digitalWrite(_NSS,HIGH);

  //Return received data
  return RFM_Data;
}

/*
*****************************************************************************************
* Description : Function for sending a package with the RFM
*
* Arguments   : *RFM_Tx_Package Pointer to arry with data to be send
*               Package_Length  Length of the package to send
*****************************************************************************************
*/

void RFM95_S::RFM_Send_Package(unsigned char *RFM_Tx_Package, unsigned char Package_Length)
{
  unsigned char i;
  // unsigned char RFM_Tx_Location = 0x00;

  //Set RFM in Standby mode wait on mode ready

  RFM_Write(0x01,0x81);
  /*
  while (digitalRead(DIO5) == LOW)
  {
  }
  */
  delay(10);

  //Switch DIO0 to TxDone
  RFM_Write(0x40,0x40);
    
  //Set IQ to normal values
  RFM_Write(0x33,0x27);
  RFM_Write(0x3B,0x1D);

  //Set payload length to the right length
  RFM_Write(0x22,Package_Length);

  //Get location of Tx part of FiFo
  //RFM_Tx_Location = RFM_Read(0x0E);

  //Set SPI pointer to start of Tx part in FiFo
  //RFM_Write(0x0D,RFM_Tx_Location);
  RFM_Write(0x0D,0x80); // hardcoded fifo location according RFM95 specs

  //Write Payload to FiFo
  for (i = 0;i < Package_Length; i++)
  {
    RFM_Write(0x00,*RFM_Tx_Package);
    RFM_Tx_Package++;
  }

  //Switch RFM to Tx
  RFM_Write(0x01,0x83);

  //Wait for TxDone:   while( digitalRead(_DIO0) == LOW )  { };
  delay(120);//Hack for extra pin, works with up to 80 Bytes of payload, with sf=10.

  //Switch RFM to sleep
  RFM_Write(0x01,0x00);
}
/*
*****************************************************************************************
* Description : Function for setting the sync word different from 0x34 (LoRaWan)
*
* Arguments   : sync_word to change to 
*               
*****************************************************************************************
*/
void RFM95_S::setSyncWord(uint8_t sync_word)
{
	RFM_Write(0x39,sync_word);
}
/*
*****************************************************************************************
* Description : Function for setting the frequency different from 868.100 MHz
*
* Arguments   : int number from 0 to 7
*               0 ist 868.100 MHz
*				1 ist 868.300 MHz
*				2 ist 868.500 MHz
*				3 ist 867.100 MHz
*				4 ist 867.300 MHz
*				5 ist 867.500 MHz
*				6 ist 867.700 MHz
*				7 ist 867.900 MHz
*****************************************************************************************
*/
void RFM95_S::setFrequencyPlan(uint8_t frequencyPlan)
{
	if (frequencyPlan < 8)
	{
		
		// EU863-870 specifications
		switch (frequencyPlan)
		{
			case 0x00: //Channel 0 868.100 MHz / 61.035 Hz = 14222987 = 0xD9068B
			RFM_Write(0x06,0xD9);
			RFM_Write(0x07,0x06);
			RFM_Write(0x08,0x8B);
			break;
			case 0x01: //Channel 1 868.300 MHz / 61.035 Hz = 14226264 = 0xD91358
			RFM_Write(0x06,0xD9);
			RFM_Write(0x07,0x13);
			RFM_Write(0x08,0x58);
			break;
			case 0x02: //Channel 2 868.500 MHz / 61.035 Hz = 14229540 = 0xD92024
			RFM_Write(0x06,0xD9);
			RFM_Write(0x07,0x20);
			RFM_Write(0x08,0x24);
			break;
			// added five more channels
			case 0x03: // Channel 3 867.100 MHz / 61.035 Hz = 14206603 = 0xD8C68B
			RFM_Write(0x06,0xD8);
			RFM_Write(0x07,0xC6);
			RFM_Write(0x08,0x8B);
			break;
			case 0x04: // Channel 4 867.300 MHz / 61.035 Hz = 14209880 = 0xD8D358
			RFM_Write(0x06,0xD8);
			RFM_Write(0x07,0xD3);
			RFM_Write(0x08,0x58);
			break;
			case 0x05: // Channel 5 867.500 MHz / 61.035 Hz = 14213156 = 0xD8E024
			RFM_Write(0x06,0xD8);
			RFM_Write(0x07,0xE0);
			RFM_Write(0x08,0x24);
			break;
			case 0x06: // Channel 6 867.700 MHz / 61.035 Hz = 14216433 = 0xD8ECF1
			RFM_Write(0x06,0xD8);
			RFM_Write(0x07,0xEC);
			RFM_Write(0x08,0xF1);
			break;
			case 0x07: // Channel 7 867.900 MHz / 61.035 Hz = 14219710 = 0xD8F9BE
			RFM_Write(0x06,0xD8);
			RFM_Write(0x07,0xF9);
			RFM_Write(0x08,0xBE);
			break;
			// FSK       868.800 Mhz => not used in this config
			// 869.525 - SF9BW125 (RX2 downlink only) for package received
		}
	}
}

/*
*****************************************************************************************
* Description : Function for setting the spreading factor
*
* Arguments   : Spreading Factor from 7 to 10
*               
*****************************************************************************************
*/
void RFM95_S::setSpreadingFactor(uint8_t sf)
{
	if (sf<11 && sf>6)
	{
		switch (sf) 
		{
			case 0x07: //SpreadingFactor 7
			RFM_Write(0x1E,0x74);
			break;
			case 0x08: //SpreadingFactor 8
			RFM_Write(0x1E,0x84);
			break;
			case 0x09: //SpreadingFactor 9
			RFM_Write(0x1E,0x94);
			break;
			case 0x0A: //SpreadingFactor 10
			RFM_Write(0x1E,0xA4);
			break;
		}
	}
}