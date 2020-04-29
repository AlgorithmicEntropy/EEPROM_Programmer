/*
 * EEPROM Programmer
 *
 * Created: 15.04.2020 19:01:55
 * Author : Sebastian
 */ 

//clock speed
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include "shift.h"
#include "serialCom.h"
#include "eeprom.h"
#include "fifo.h"

//command code bytes
#define enableWrite 0x01
#define setReadStartAdd 0x02
#define enableRead 0x04
#define setReadEndAdd 0x6

//global vars
uint8_t writeMode = 0;
volatile uint16_t readStartAddress;
volatile uint16_t readEndAddress;
uint8_t command[3];

//function prototypes
void EvalCommand(void);

int main(void)
{
	//startup code - setup pin modes etc
	setupShiftIO();
	setupSerial();
	setupEEPROM();
	setupFIFO();
	
	//enable interrupts
	sei();
	
	//indicate successful startup, ready for data
	SEND_ACK();
	
	//loop and wait for serial interrupt
	while (1)
	{
		//flow control paused and buffer getting empty --> reenable serial comm with XON
		if (!FLOW_STATUS && buffer.counter < BUFFER_THREASHOLD_LOWER)
		{
			FLOW_STATUS = 1;
			_USART_send(XON);
		}
				
		//only run if buffer contains at least 3 bytes
		if (buffer.counter < 3)
		{
			continue;
		}
		
		//disable interrupts -> atomic reading from fifo
		cli();
		//read 3 bytes
		BufferOut(command);
		BufferOut(command+1);
		BufferOut(command+2);
		//re-enable interrupts
		sei();
		//parse 3 byte command
		EvalCommand();
	}
}

//evaluate command buffer for instructions
void EvalCommand()
{
	if (writeMode)
	{
		if (command[0] != 0xFF) //--> valid eeprom address -> max 15bit -> 0xff as address high-byte would be invalid
		{
			//write command data to eeprom
			writeBulkData(command);
			//writeSingleByte(command);
			//_delay_ms(8);
		}
		else
		{
			//leave write mode
			writeMode = 0;
			//confirm mode switch with ACK
			SEND_ACK();
		}
	}
	
	else if (command[0] == enableWrite)
	{
		//switch programmer to write mode
		writeMode = 1;
		//confirm mode switch with ACK
		SEND_ACK();
	}
	else if (command[0] == enableRead)
	{
		if (readStartAddress < readEndAddress)
		{
			readEEPROM(readStartAddress, readEndAddress);
			//read finished - confirm with ACK
			SEND_ACK();
		}
		else
		{
			//invalid address configuration
			SEND_ERR();
		}
	}
	else if (command[0] == setReadStartAdd)
	{
		//build 16 bit address from 2 bytes
		readStartAddress = 0;
		readStartAddress |= (uint16_t)command[1] << 8;
		readStartAddress |= (uint16_t)command[2];
		SEND_ACK();
	}
	else if (command[0] == setReadEndAdd)
	{
		//build 16 bit address from 2 bytes
		readEndAddress = 0;
		readEndAddress |= (uint16_t)command[1] << 8;
		readEndAddress |= (uint16_t)command[2];
		SEND_ACK();
	}
	else
	{
		SEND_ERR();
	}
}




