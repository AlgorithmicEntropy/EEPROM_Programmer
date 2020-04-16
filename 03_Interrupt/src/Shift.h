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

void shiftOut(char);
void setupShiftIO(void);

void shiftOut(char data)
{
	for (char i = 0; i < 8; i++)
	{
		//<< 7-i reverse order
		if (data & (1 << i))
		{
			Data_HIGH();
		}
		else
		{
			Data_LOW();
		}
		
		//shift by pulsing clock high
		Clock_HIGH();
		Clock_LOW();
	}
	Latch_HIGH();
	Latch_LOW();
}

void setupShiftIO()
{
	//set pins as output
	DDRD |= lPin | cPin | dPin;
}




#endif /* SHIFT_H_ */