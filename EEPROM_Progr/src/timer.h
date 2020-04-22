/*
 * timer.h
 *
 * Created: 20.04.2020 14:10:21
 *  Author: Sebastian
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define top 3397 //0.01s at 16MHz clk //63974 = 1s

void setupTimer(void);

void setupTimer()
{
	TCCR1A = 0x00;
	TCCR1B = (1<<CS10) | (1<<CS12);;  // Timer mode with 1024 prescaler
	TCNT1 = top; //set top value
	TIMSK1 = (1 << TOIE1);
}




#endif /* TIMER_H_ */