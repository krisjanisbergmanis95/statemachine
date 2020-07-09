#define F_CPU 16000000UL

#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <avr/interrupt.h>



void PortInit();
void USARTInit(int mode);
void USART_Transmit(char val);
void USART_Transmit_String(char *);
int main(void)
{
	PortInit();

	USARTInit(1);
	sei ();
	
	while (1)
	{
		//USART_Transmit('a');
	USART_Transmit_String("test hello this is a message");
	}
}

/*-------------INIT----------------*/
void PortInit()
{
	DDRD = 0xFF;
	DDRB = 0x2F;
}

void USARTInit(int mode) {
	// 	datu biti: 8
	// STOP biti: 1
	// paritāte: nav
	// baud rate: 9600
	UBRR0 = 103; //9600
	if (mode == 0) { //receive moe
		UCSR0B = (1 << RXEN0) | (1 << RXCIE0); //enable reciever and receive interupt
		UCSR0C = (1 << UCSZ00) | (1 <<  UCSZ01); //8 data bits
	} else if (mode == 1) {//transmit mode
		UCSR0B = (1 << TXEN0) ; //enable transmitter
		UCSR0C = (1 << UCSZ00) | (1 <<  UCSZ01); //8 data bits
	}	
}

/*^^------------INIT----------------*/

/*-------------ISR----------------*/

/*^^-----------ISR----------------*/


void USART_Transmit(char val) {

	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = val;
	
}

void USART_Transmit_String(char *val) {
	while(*val != '\0') {
		USART_Transmit(*val);
		val++;
	}
}
