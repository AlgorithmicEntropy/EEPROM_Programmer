/*
 * Shift.h
 *
 * Created: 16.04.2020 14:49:26
 *  Author: Sebastian
 */ 


#ifndef SHIFT_H_
#define SHIFT_H_


#include <avr/io.h>
#include <util/delay.h>

//pin definitions
#define lPin 0b00001000 //Pin D3 latch
#define cPin 0b00010000 //Pin D4 clock
#define dPin 0b00100000 //Pin D5 data out

//pin macros
#define Latch_HIGH() PORTD |= lPin
#define Latch_LOW() PORTD &= ~lPin
#define Clock_HIGH() PORTD |= cPin
#define Clock_LOW() PORTD &= ~cPin
#define Data_HIGH() PORTD |= dPin
#define Data_LOW() PORTD &= ~dPin

#define Latch_Pulse() Latch_HIGH(); Latch_LOW();

enum BITORDER {MSBFIRST, LSBFIRST};
typedef enum BITORDER bitorder;

void shiftOutByte(uint8_t, uint8_t, bitorder);
void shiftOutShort(uint16_t, bitorder);
void setupShiftIO(void);

void shiftOutByte(uint8_t data, uint8_t autoLatch, bitorder order)
{
	for (uint8_t i = 0; i < 8; i++)
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

void shiftOutShort(uint16_t data, bitorder order)
{
	//shift out HIGHBYTE
	shiftOutByte(data >> 8, 0, order);
	//shift out LOWBYTE
	shiftOutByte(data, 0, order);
	//latch outputs
	Latch_Pulse();
}

void setupShiftIO()
{
	//set pins as output
	DDRD |= lPin | cPin | dPin;
}




#endif /* SHIFT_H_ */