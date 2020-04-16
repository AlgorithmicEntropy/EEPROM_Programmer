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

#define BAUD_RATE_9600_BPS  103  // 9600bps

void setupSerial(void);
char USART_receive(void);
void USART_send(char);
void USART_putString(char[]);

void setupSerial()
{
	UBRR0H = BAUD_RATE_9600_BPS >> 8;
	UBRR0L = BAUD_RATE_9600_BPS;
	
	UCSR0A &= ~(1 << U2X0);	//disable double async mode
	UCSR0B |= (1 << RXCIE0);		//enable receiver interupts
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);		// enable rx and tx
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);		//framing  8data, 1stop
		
	sei();
}

char USART_receive()
{
	while(!(UCSR0A & (1 << 5)))		//wait for buffer to finish
	{
		
	}
	
	return UDR0;
}

void USART_send(char data)
{
	while(!(UCSR0A & (1 << 5)))		//wait for buffer to finish
	{
		
	}
	
	UDR0 = data;
}

void USART_putString(char str[])
{
	int i = 0;
	while (str[i] != 0x00)
	{
		USART_send(str[i]);
		i++;
	}
}


#endif /* SERIALCOM_H_ */