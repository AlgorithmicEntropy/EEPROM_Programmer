/*
 * eeprom.h
 *
 * Created: 16.04.2020 22:04:57
 *  Author: Sebastian
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>

#define WE 0b00000100		//pin D3, port D - write enable
#define D0 0b01000000		//pin D6, port D
#define D1 0b10000000		//pin D7, port D
#define D2 0b00000001		//pin D8, port B
#define D3 0b00000010		//pin D9, port B
#define D4 0b00000100		//pin D10, port B
#define D5 0b00001000		//pin D11, port B
#define D6 0b00010000		//pin D12, port B
#define D7 0b00100000		//pin D13, port B

#define PortD_Mask 0b11000000
#define PortB_Mask 0b00111111

#define writeEnable_HIGH() PORTD |= WE
#define writeEnable_LOW() PORTD &= ~WE

void setupEEPROM(void);
void writeEEPROM(unsigned char[]);
void readEEPROM(int startAddress, int endAddress);
void _writeDataByte(unsigned char);
void _setInputMode(void);
void _setOutputMode(void);
unsigned char _readDataByte(void);

char IO_Mode = 0;

void setupEEPROM()
{
	//set pin for read mode
	_setInputMode();
	
}

void writeEEPROM(unsigned char lineData[3])
{
	//check for correct pin mode
	if (!IO_Mode)
	{
		_setOutputMode();
	}
	//shift address
	for (int i = 0; i < 2; i++)
	{
		shiftOut(lineData[i]);
	}
	
	//write data byte
	_writeDataByte(lineData[2]);
}

void readEEPROM(int startAddress, int endAddress)
{
	//check for correct pin mode
	if (IO_Mode)
	{
		_setInputMode();
	}
	
	for (int i = startAddress; i < endAddress; i++)
	{
		//shift address low and high byte
		shiftOut(i << 8);
		shiftOut(i);
		
		//read byte and send via serial
		USART_send(_readDataByte());
	}
}

void _writeDataByte(unsigned char data)
{

	
	//set portD
	PORTD &= ~PortD_Mask;
	PORTD |= (data & PortD_Mask);
	
	//set portB
	PORTB &= (data & PortB_Mask);     
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
	DDRD &= ~(D0 | D1);
	//set DDRB to input for pin 8 to 13
	DDRB &= ~(D2 | D3 | D4 | D5 | D6 | D7);
	
	IO_Mode = 0;
}

void _setOutputMode()
{
	DDRD |= D0 | D1;
	DDRB |= D2 | D3 | D4 | D5 | D6 | D7;
	
	IO_Mode = 1;
}



#endif /* EEPROM_H_ */