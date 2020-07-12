#define F_CPU 16000000UL

#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/delay.h>


void PortInit();
void Init_ExternalInterupts();
void USARTInit(int mode);
void USART_Transmit(char val);
void USART_Transmit_String(char *);
uint8_t USART_Read_Byte;
enum INPUT {coin, push, waiting};
enum INPUT input_action = waiting;
	
enum STATE {locked, unlocked};
enum STATE next_state = locked;

void checkState();
void checkInput();

int write_mode = 0;
int main(void){

	
	PortInit();
USARTInit(0);
	//USARTInit(1);
	sei ();
	
	while (1) {
		USARTInit(0);
		checkInput();
// 		if (write_mode == 1) {
// 			USARTInit(1);
// 			checkState();
// 			write_mode = 0;
// 		}
		USARTInit(1);
		checkState();
		
		//USART_Transmit_String("12");
	}

}

void checkState() {
	write_mode = 0;
	switch(next_state) {
		
		
		case locked:
		if (input_action == coin) {
			next_state = unlocked;
			input_action = waiting;
			USARTInit(1);
			USART_Transmit_String("A");
// 			USART_Transmit_String("current state: locked");
// 			USART_Transmit_String("next state: unlocked");
// 			USART_Transmit_String("unlocked - you can go");
		}
		else if (input_action == push) {
			next_state = locked;
			input_action = waiting;
			USARTInit(1);
			USART_Transmit_String("B");
// 			USART_Transmit_String("current state: locked");
// 			USART_Transmit_String("next state: locked");
// 			USART_Transmit_String("locked - insert coin");
		}
		break;
		case unlocked:
		if (input_action == coin) {
			next_state = unlocked;
			input_action = waiting;
			USARTInit(1);
			USART_Transmit_String("C");
// 			USART_Transmit_String("current state: unlocked");
// 			USART_Transmit_String("next state: unlocked");
// 			USART_Transmit_String("unlocked already - you can go");
		}
		else if (input_action == push) {
			next_state = locked;
			input_action = waiting;
			USARTInit(1);
			USART_Transmit_String("D");
// 			USART_Transmit_String("current state: unlocked");
// 			USART_Transmit_String("next state: locked");
// 			USART_Transmit_String("enjoy. locking after");
		}
		break;
		USARTInit(0);
	}

}

void checkInput() {
	if (USART_Read_Byte == 'c') {
		input_action = coin;
		
	} else if (USART_Read_Byte == 'p') {
		input_action = push;
		
	}
	else {
		input_action = waiting;
		//write_mode = 0;
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

Init_ExternalInterupts(){
	EICRA |= (1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00);
	EIMSK |= (1<< INT1) | (1 <<INT0);
}

/*^^------------INIT----------------*/

/*-------------ISR----------------*/
ISR(INT0_vect) {
	input_action = coin;
}

ISR(INT1_vect) {
	input_action = push;
}

ISR(USART_RX_vect){
	write_mode = 1;
	
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
