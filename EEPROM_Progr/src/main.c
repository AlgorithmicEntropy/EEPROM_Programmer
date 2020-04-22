/*
 * EEPROM Programmer
 *
 * Created: 15.04.2020 19:01:55
 * Author : Sebastian
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "shift.h"
#include "serialCom.h"
#include "eeprom.h"
#include "timer.h"
#include "fifo.h"

#define startByte 0x55
#define writeByte 0x01
#define readByte 0x02

char idleMode = 1;
char writeMode = 0;
char readMode = 0;
unsigned short startAddress;
uint8_t command[3];


void EvalCommand(void);

int main(void)
{
	//startup code - setup pin modes etc
	setupShiftIO();
	setupSerial();
	setupEEPROM();
	setupTimer();
	setupFIFO();
	
	//enable interrupts
	sei();
	
	//indicate successful startup, ready for data
	USART_putByte(startByte);
	
	//loop and wait for serial interrupt
	while (1)
	{
		//only run if buffer contains valid frames
		if (buffer.counter < 3)
		{
			continue;
		}
		
		if (!FLOW_STATUS && buffer.counter < BUFFER_THREASHOLD_LOWER)
		{
			FLOW_STATUS = 1;
			_USART_send(XON);
		}
		
		//disable interrupts -> atomic reading from fifo
		cli();
		//read 3 bytes
		BufferOut(command);
		BufferOut(command+1);
		BufferOut(command+2);
		//re-enable interrupts
		sei();
		
		if (idleMode && command[0] == startByte)
		{
			idleMode = 0;
			USART_putByte(startByte);
		}
		
		else if (!idleMode)
		{
			//if read or write mode --> direct action
			if (writeMode)
			{
				if (command[0] == 0xFF)
				{
					//0xFF as address highbyte -> invalid read address
					//process finished - send to sleep mode
					idleMode = 1;
					writeMode = 0;
					USART_putByte(startByte);
				}
				else
				{
					//in write mode --> write data to eeprom
					writeEEPROM(command);
				}
			}
			else if (readMode)
			{
				//get end address from two bytes
				unsigned short endAddress = 0;
				endAddress |= (unsigned short)command[1] << 8;
				endAddress |= (unsigned short)command[2];
				
				//read eeprom between addresses
				readEEPROM(startAddress, endAddress);
				
				//reset state - confirm op done with start code
				idleMode = 1;
				readMode = 0;
				USART_putByte(startByte);
			}
			else
			{
				//no defined state - check received data for instructions
				EvalCommand();
			}
		}
	}
}

//timer 1 ISR
ISR(TIMER1_OVF_vect)
{
	
}



//evaluate command buffer for instructions
void EvalCommand()
{
	if (command[0] == writeByte)
	{
		//write mode confirmed - route write further data to eeprom
		writeMode = 1;
		USART_putByte(startByte);
	}
	else if (command[0] == readByte)
	{
		// read mode confirmed - save start address
		readMode = 1;
		startAddress |= (unsigned short)command[1] << 8;
		startAddress |= (unsigned short)command[2];

		//confirm read mode
		USART_putByte(startByte);
	}

}




