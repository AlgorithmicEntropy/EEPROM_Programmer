/*
 * Shift.h
 *
 * Created: 16.04.2020 14:49:26
 *  Author: Sebastian
 */ 


#ifndef SHIFT_H_
#define SHIFT_H_

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define lPin 0b00001000 //Pin D3 latch
#define cPin 0b00010000 //Pin D4 clock
#define dPin 0b00100000 //Pin D5 data out

#define Latch_HIGH() PORTD |= lPin
#define Latch_LOW() PORTD &= ~lPin
#define Clock_HIGH() PORTD |= cPin
#define Clock_LOW() PORTD &= ~cPin
#define Data_HIGH() PORTD |= dPin
#define Data_LOW() PORTD &= ~dPin

#define Latch_Pulse() Latch_HIGH(); Latch_LOW();

enum BITORDER {MSBFIRST, LSBFIRST};
typedef enum BITORDER bitorder;

void shiftOutChar(char, char, bitorder order);
void shiftOutShort(unsigned short);
void setupShiftIO(void);

void shiftOutChar(char data, char autoLatch, bitorder order)
{
	for (char i = 0; i < 8; i++)
	{
		if (order)
		{
			if (data & (1 << i))
			{
				Data_HIGH();
			}
			else
			{
				Data_LOW();
			}
		}
		else
		{
		//<< 7-i reverse order
			if (data & (1 << (7-i)))
			{
				Data_HIGH();
			}
			else
			{
				Data_LOW();
			}			
		}
		
		//shift by pulsing clock high
		Clock_HIGH();
		Clock_LOW();
	}
	
	if (autoLatch)
	{
		//Pulse Shift Register Latch Pin to present value on outputs
		Latch_Pulse();
	}
}

void shiftOutShort(unsigned short data)
{
	//shift out HIGHBYTE
	shiftOutChar(data >> 8, 0, MSBFIRST);
	//shift out LOWBYTE
	shiftOutChar(data, 0, MSBFIRST);
	//latch outputs
	Latch_Pulse();
}

void setupShiftIO()
{
	//set pins as output
	DDRD |= lPin | cPin | dPin;
}




#endif /* SHIFT_H_ */