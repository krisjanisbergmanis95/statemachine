#define F_CPU 16000000UL

#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/delay.h>


void PortInit();
void Init_ExternalInterupts();
void USARTInit();
void USART_Transmit(char val);
void USART_Transmit_String(char *);
uint8_t USART_Read_Byte;
uint8_t test_int = 0;
enum INPUT {coin, push, waiting};
enum INPUT input_action = waiting;

enum STATE {locked, unlocked};
enum STATE next_state = locked;

void checkState();
void checkInput();

int main(void){
	USARTInit();
	
	PortInit();

	sei ();
	
	while (1) {
	//USART_Transmit_String("test");
		checkInput();
		checkState();
	}

}

void checkState() {
	//USART_Transmit_String("check state");
	switch(next_state) {
		
		
		case locked:
		if (input_action == coin) {
			next_state = unlocked;
			input_action = waiting;
			
			USART_Transmit_String("A");
			USART_Transmit_String("current state: locked\n");
			USART_Transmit_String("next state: unlocked\n");
			USART_Transmit_String("unlocked - you can go\n");
		}
		else if (input_action == push) {
			next_state = locked;
			input_action = waiting;
			
			USART_Transmit_String("B");
			USART_Transmit_String("current state: locked\n");
			USART_Transmit_String("next state: locked\n");
			USART_Transmit_String("locked - insert coin\n");
		}
		break;
		case unlocked:
		if (input_action == coin) {
			next_state = unlocked;
			input_action = waiting;
			
			USART_Transmit_String("C");
			USART_Transmit_String("current state: unlocked\n");
			USART_Transmit_String("next state: unlocked\n");
			USART_Transmit_String("unlocked already - you can go\n");
		}
		else if (input_action == push) {
			next_state = locked;
			input_action = waiting;
			
			USART_Transmit_String("D");
			USART_Transmit_String("current state: unlocked\n");
			USART_Transmit_String("next state: locked\n");
	 		USART_Transmit_String("enjoy. locking after\n");
		}
		break;
	
	}

}

void checkInput() {
	test_int = test_int ++;
	if (USART_Read_Byte == 'c') {
		input_action = coin;
		
	} else if (USART_Read_Byte == 'p') {
		input_action = push;
		
	}
	else {
		input_action = waiting;
	}
}


/*-------------INIT----------------*/
void PortInit()
{
	DDRD = 0b00000000;
	PORTD = 0b00001100;
	DDRB = 0b00100000;
	PORTB = 0b00000000;
}

void USARTInit() {
	// 	datu biti: 8
	// STOP biti: 1
	// parit?te: nav
	// baud rate: 9600
	UBRR0 = 103; //9600
	UCSR0B = (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0) ; //enable reciever and receive interupt
	UCSR0C = (1 << UCSZ00) | (1 <<  UCSZ01); //8 data bits
}

Init_ExternalInterupts(){
	EICRA |= (1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00);
	EIMSK |= (1<< INT1) | (1 <<INT0);
}

/*^^------------INIT----------------*/

/*-------------ISR----------------*/

ISR(USART_RX_vect){
	USART_Read_Byte = UDR0;
	PORTB ^= (1 << PORTB5);
}
/*^^-----------ISR----------------*/


void USART_Transmit(char val) {

	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = val;
	
}

void USART_Transmit_String(char *val) {
	while(*val != NULL) {//'\0'
		USART_Transmit(*val);
		val++;
	}
}
