/*
 * serialCom.h
 *
 * Created: 16.04.2020 17:49:04
 *  Author: Sebastian
 */ 


#ifndef SERIALCOM_H_
#define SERIALCOM_H_

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "fifo.h"

#define BAUD_RATE_9600_BPS  103  // 9600bps
#define BAUD_RATE_57600_BPS 16

#define startByte 0x55
#define XON 0x11
#define XOFF 0x13
#define ESCAPE_CHAR 0x0F //backslash

char FLOW_STATUS = 1;

void setupSerial(void);
void pauseData(void);
void resumeData(void);
char _USART_receive(void);
void _USART_send(uint8_t);
void USART_putString(uint8_t[]);
void USART_putByte(uint8_t);

void setupSerial()
{
	UBRR0H = BAUD_RATE_57600_BPS >> 8;
	UBRR0L = BAUD_RATE_57600_BPS;
	
	UCSR0A &= ~(1 << U2X0);	//disable double async mode
	UCSR0B |= (1 << RXCIE0);		//enable receiver interupts
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);		// enable rx and tx
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);		//framing  8data, 1stop
}

//USART serial receive interrupt routine
ISR(USART_RX_vect)
{
	//check buffer status, send stop byte if getting full
	if (buffer.counter > BUFFER_THREASHOLD_UPPER)
	{
		_USART_send(XOFF);
		FLOW_STATUS = 0;
	}
	
	BufferIn(_USART_receive());
}

char _USART_receive()
{
	while(!(UCSR0A & (1 << 5)))		//wait for buffer to finish
	{
		
	}
	
	return UDR0;
}

void _USART_send(uint8_t data)
{
	while(!(UCSR0A & (1 << 5)))		//wait for buffer to finish
	{
		
	}
	
	UDR0 = data;
}

void USART_putByte(uint8_t data)
{
	//check need to escape
	if (data == XOFF || data == XON || data == ESCAPE_CHAR)
	{
		_USART_send(ESCAPE_CHAR);
		_USART_send(~data);
	}
	else
	{
		_USART_send(data);
	}

}

void USART_putString(uint8_t str[])
{
	int i = 0;
	while (str[i] != 0x00)
	{
		USART_putByte(str[i]);
		i++;
	}
}


#endif /* SERIALCOM_H_ */