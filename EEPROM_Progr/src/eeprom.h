/*
 * eeprom.h
 *
 * Created: 16.04.2020 22:04:57
 *  Author: Sebastian
 *
 *	Implementation to read and write data from an EEPROM
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>
#include "shift.h"
#include "fifo.h"

#define WE 0b00000100		//pin D2, port D - write enable

#define D0 0b00000001		//pin D8, port B
#define D1 0b00000010		//pin D9, port B
#define D2 0b00000100		//pin D10, port B
#define D3 0b00001000		//pin D11, port B
#define D4 0b00010000		//pin D12, port B
#define D5 0b00100000		//pin D13, port B
#define D6 0b01000000		//pin D6, port D
#define D7 0b10000000		//pin D7, port D

#define PortD_Mask 0b11000000 //used pins on portD
#define PortB_Mask 0b00111111 //used pins on portB

#define EEPROM_PAGE_MASK 0b0111111111000000 // mask page bits

//makros
#define writeEnable_HIGH() PORTD |= WE
#define writeEnable_LOW() PORTD &= ~WE

//function prototypes
void setupEEPROM(void);
void writeBulkData(uint8_t *);
void readEEPROM(uint16_t startAddress, uint16_t endAddress);
static void writeSingleByte(uint8_t, volatile uint16_t);
static void _writeDataByte(uint8_t);
static uint8_t _readDataByte(void);
static void _setInputMode(void);
static void _setOutputMode(void);

//global vars
char IO_Mode = 0;
volatile uint16_t lastAddress = 0;

//setup code - set pin modes
void setupEEPROM()
{
	//set pin for read mode
	_setInputMode();
	
	//set we direction reg
	DDRD |= WE;
	//set we high
	writeEnable_HIGH();
	
}

void writeSingleByte(uint8_t data, volatile uint16_t address)
{
	//check for correct pin mode
	if (!IO_Mode)
	{
		_setOutputMode();
	}
	//create address
	//volatile uint16_t address = start[0] << 8 | start[1] | 1 << 15;
	//shift out address
	shiftOutShort(address, MSBFIRST);
	//_delay_ms(0.01)
	lastAddress = address;
	//write data byte
	_writeDataByte(data);
}

void writeBulkData(uint8_t *start)
{
	//build address, set bit 15 high, used for output enable
	volatile uint16_t address = (start[0]) << 8 | start[1] | 1 << 15;
	//check page
	if ((lastAddress & EEPROM_PAGE_MASK) == (address & EEPROM_PAGE_MASK))
	{
		//write byte without delay
		writeSingleByte(start[2], address);
		//end bulk write cycle if buffer empty - give eeprom time to end write cycle
		if (buffer.counter < 3)
		{
			_delay_ms(10);
		}
	}
	else
	{
		//page validation or max bytes written -  give eeprom time to end write cycle
		_delay_ms(10);
		writeSingleByte(start[2], address);
	}
}

void readEEPROM(uint16_t startAddress, uint16_t endAddress)
{
	//check for correct pin mode
	if (IO_Mode)
	{
		_setInputMode();
	}
	
	//read whole eeprom
	for (volatile uint16_t i = startAddress; i < endAddress; i++)
	{
		//shift address high and low byte and set bit 15 low --> output enable
		shiftOutShort(i & ~(1 << 15), MSBFIRST);
		//read byte and send via serial
		USART_putByte(_readDataByte());
		_delay_ms(0.1);
	}
}

static void _writeDataByte(uint8_t data)
{
	//set portD
	PORTD &= ~PortD_Mask;
	PORTD |= (data & PortD_Mask);
	////set portB
	PORTB &= ~PortB_Mask;
	PORTB |= (data & PortB_Mask);
	
	//latch add --> falling edge
	writeEnable_LOW();
	//latch data --> rising edge
	writeEnable_HIGH();
}

static uint8_t _readDataByte()
{
	//read single byte from eeprom
	uint8_t data = 0;
	data |= (PIND & PortD_Mask);
	data |= (PINB & PortB_Mask);
	return data;
}

static void _setInputMode()
{
	//set DDRD to input mode for pin 6 & 7
	DDRD &= ~(D6 | D7);
	//set DDRB to input for pin 8 to 13
	DDRB &= ~(D0| D1 | D2 | D3 | D4 | D5);
	//set indicator flag
	IO_Mode = 0;
}

static void _setOutputMode()
{
	//set DDRD to output mode for pin 6 & 7
	DDRD |= D6 | D7;
	//set DDRB to input for pin 8 to 13
	DDRB |= D0 | D1 | D2 | D3 | D4 | D5;
	//set indicator flag
	IO_Mode = 1;
}



#endif /* EEPROM_H_ */