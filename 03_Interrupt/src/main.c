/*
 * EEPROM Programmer
 *
 * Created: 15.04.2020 19:01:55
 * Author : Sebastian
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "shift.h"
#include "serialCom.h"
#include "eeprom.h"

#define startByte 0x55
#define writeByte 0x01
#define readByte 0x02
#define addressFlag = 0x04

bool idleMode = true;
bool writeMode = false;
unsigned char commandBuffer[3];
unsigned char bufferCounter = 0;

int byteCounter = 0;

void EvalCommand(void);

int main(void)
{
	//startup code - setup pin modes etc
	setupShiftIO();
	setupSerial();
	setupEEPROM();
	
	//indicate successful startup, ready for data
	shiftOut(0x55);
	
	//loop and wait for serial interrupt
	//while (1)
	//{
	//}
	
	while (1)
	{
		for (char i = 0; i < 256; i++)
		{
			shiftOut(i);
			shiftOut(i);
			_delay_ms(100);
		}
	}
}

ISR(USART_RX_vect)
{
	unsigned char data = USART_receive();
	
	//valid start byte received - listening for full command
	if (idleMode && data == startByte)
	{
		idleMode = false;
	}
	
	else if (!idleMode)
	{
		if (bufferCounter < 3)
		{
			//write received byte do buffer array and increase counter
			commandBuffer[bufferCounter] = data;
			bufferCounter++;
		}
		else
		{
			if (writeMode)
			{
				//in write mode --> write data to eeprom
				writeEEPROM(commandBuffer);
			}
			else
			{
				//no defined state - check received data for instructions
				EvalCommand();
			}
			
			//reset counter
			bufferCounter = 0;
		}
	}
}

//evaluate command buffer for instructions
void EvalCommand()
{
	if (commandBuffer[0] == writeByte)
	{
		//write mode confirmed - route write further data to eeprom
		writeMode = true;
	}
	else if (commandBuffer[0] == readByte)
	{
		// read mode confirmed - read eeprom between given addresses
		readEEPROM(commandBuffer[1], commandBuffer[2]);
	}
	else if (commandBuffer[0] == startByte)
	{
		//process finished - send to sleep mode
		idleMode = true;
		writeMode = false;
	}
}




