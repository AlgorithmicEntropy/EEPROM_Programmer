/*
 * eeprom.h
 *
 * Created: 16.04.2020 22:04:57
 *  Author: Sebastian
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>
#include "shift.h"

#define WE 0b00000100		//pin D2, port D - write enable

#define D0 0b00000001		//pin D8, port B
#define D1 0b00000010		//pin D9, port B
#define D2 0b00000100		//pin D10, port B
#define D3 0b00001000		//pin D11, port B
#define D4 0b00010000		//pin D12, port B
#define D5 0b00100000		//pin D13, port B
#define D6 0b01000000		//pin D6, port D
#define D7 0b10000000		//pin D7, port D

#define PortD_Mask 0b11000000
#define PortB_Mask 0b00111111

#define writeEnable_HIGH() PORTD |= WE
#define writeEnable_LOW() PORTD &= ~WE

void setupEEPROM(void);
void writeEEPROM(unsigned char[]);
void readEEPROM(unsigned short startAddress, unsigned short endAddress);
void _writeDataByte(unsigned char);
void _setInputMode(void);
void _setOutputMode(void);
unsigned char _readDataByte(void);

char IO_Mode = 0;

void setupEEPROM()
{
	//set pin for read mode
	_setInputMode();
	
	//set we direction reg
	DDRD |= WE;
	//set we high
	writeEnable_HIGH();
	
}

void writeEEPROM(unsigned char *start)
{
	//check for correct pin mode
	if (!IO_Mode)
	{
		_setOutputMode();
	}
	//shift address
	register unsigned char add_H = *start;
	start++;
	register unsigned char add_L = *start;
	start++;
	shiftOutShort((add_H << 8) | (unsigned short)(add_L) | 1 << 15);
	
	//write data byte
	_writeDataByte(*start);
}

void readEEPROM(unsigned short startAddress, unsigned short endAddress)
{
	//check for correct pin mode
	if (IO_Mode)
	{
		_setInputMode();
	}
	
	for (register int i = startAddress; i < endAddress; i++)
	{
		//shift address high and low byte
		shiftOutShort(i & ~(1 << 15));
		//read byte and send via serial
		USART_putByte(_readDataByte());
		
		_delay_ms(0.1);
	}
}

void _writeDataByte(unsigned char data)
{
	//set portD
	PORTD &= ~PortD_Mask;
	PORTD |= (data & PortD_Mask);
	//
	////set portB
	PORTB &= ~PortB_Mask;
	PORTB |= (data & PortB_Mask);
	
	
	//latch add --> falling edge
	writeEnable_LOW();
	_delay_ms(0.21);
	//latch data --> rising edge
	writeEnable_HIGH();
	
	//sleep for write to end
	_delay_ms(10);
	_setInputMode();
	while (_readDataByte() == ~data)
	{
		USART_putByte(0xFF);
	} 
}

unsigned char _readDataByte()
{
	unsigned char data = 0;
	data |= (PIND & PortD_Mask);
	data |= (PINB & PortB_Mask);
	return data;
}

void _setInputMode()
{
	//set DDRD to input mode for pin 6 & 7
	DDRD &= ~(D6 | D7);
	//set DDRB to input for pin 8 to 13
	DDRB &= ~(D0| D1 | D2 | D3 | D4 | D5);

	IO_Mode = 0;
}

void _setOutputMode()
{
	DDRD |= D6 | D7;
	DDRB |= D0 | D1 | D2 | D3 | D4 | D5;
	
	IO_Mode = 1;
}



#endif /* EEPROM_H_ */