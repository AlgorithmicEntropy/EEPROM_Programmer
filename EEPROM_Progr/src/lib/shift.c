/*
 * shift.c
 *
 * Created: 05.05.2020 18:51:43
 *  Author: Sebastian
 */ 


#include <avr/io.h>
#include "shift.h"
#define F_CPU 16000000UL
#include <util/delay.h>


//setup code
void setupShiftIO()
{
	//set pins as output
	DDRD |= lPin | cPin | dPin;
}

//shift out a single byte
void shiftOutByte(uint8_t data, uint8_t autoLatch, bitorder order)
{
	//loop for 8 bits
	for (uint8_t i = 0; i < 8; i++)
	{
		//check bitorder
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
			//reverse order
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