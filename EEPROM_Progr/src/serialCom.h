/*
 * serialCom.h
 *
 * Created: 16.04.2020 17:49:04
 *  Author: Sebastian
 *
 *	USART serial communication protocol Implementation using receive fifo buffer
 */ 


#ifndef SERIALCOM_H_
#define SERIALCOM_H_

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "fifo.h"

#define BAUD_RATE_9600_BPS  103  // 9600bps @ 16Mhz
#define BAUD_RATE_57600_BPS 16   //57600bps @ 16Mhz

//status bytes
#define XON 0x11	//flow control XON
#define XOFF 0x13	//flow control XOFF
#define ACK 0x55	//ACK byte - confirm
#define ERR 0x12	//ERR byte -  error
#define ESCAPE_CHAR 0x0F //escape sequence

//status macros
#define SEND_XON() _USART_send(XON)
#define SEND_XOFF() _USART_send(XOFF)
#define SEND_ACK() _USART_send(ACK)
#define SEND_ERR() _USART_send(ERR)

//global vars
uint8_t FLOW_STATUS = 1;	//flow control status indicator

//function prototypes
void setupSerial(void);
static uint8_t _USART_receive(void);
static void _USART_send(uint8_t);
void USART_putString(char *);
void USART_putByte(uint8_t);

void setupSerial()
{
	UBRR0H = BAUD_RATE_9600_BPS >> 8;
	UBRR0L = BAUD_RATE_9600_BPS;
	
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

static uint8_t _USART_receive()
{
	while(!(UCSR0A & (1 << 5)))		//wait for buffer to finish
	{
		
	}
	
	return UDR0;
}

static void _USART_send(uint8_t data)
{
	while(!(UCSR0A & (1 << 5)))		//wait for buffer to finish
	{
		
	}
	
	UDR0 = data;
}

void USART_putByte(uint8_t data)
{
	//check need to escape
	if (data == XOFF || data == XON || data == ESCAPE_CHAR || data == ACK || data == ERR)
	{
		_USART_send(ESCAPE_CHAR);
		_USART_send(~data);
	}
	else
	{
		_USART_send(data);
	}
}

void USART_putString(char *str)
{
	while (*str != 0x00)
	{
		USART_putByte(*str);
		str++;
	}
}


#endif /* SERIALCOM_H_ */