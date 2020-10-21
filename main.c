#define F_CPU 16000000UL

#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/delay.h>


void PortInit();
void Init_ExternalInterupts();
void USARTInit();
void USART_Transmit(char val);
void USART_Transmit_String(char *);
uint8_t USART_Read_Byte;
char USARTReadBufferArr[16] = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
int usart_read_index = 0;

char *LEVEL_ONE_PIN = "123";
char pin_to_check_level_one[3] = {};

uint8_t test_int = 0;
enum INPUT {coin, push, waiting};
enum INPUT input_action = waiting;

enum STATE {locked, unlocked};
enum STATE next_state = locked;

void checkState();
void checkInput();
void echo();
void clear_pin_to_check_level_one();

int main(void){
	USARTInit();
	
	PortInit();

	sei ();
	
	while (1) {
		checkInput();
		checkState();
	}

}

void checkState() {
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
	//input_action = waiting;

}

void checkInput() {
	int result = 11000;
	result = strcmp(pin_to_check_level_one,LEVEL_ONE_PIN);

	if (result == 0) {
		input_action = coin;
		clear_pin_to_check_level_one();
		
	} else if (result != 0 && pin_to_check_level_one[0] != NULL && pin_to_check_level_one[1] != NULL) {
		input_action = push;
		clear_pin_to_check_level_one();
		}
	else {
		input_action = waiting;
	}
	_delay_ms(500);
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
	pin_to_check_level_one[usart_read_index] = UDR0;
	usart_read_index++;
	
	if (usart_read_index > 2) {
		usart_read_index = 0;
	}
}
/*^^-----------ISR----------------*/


void USART_Transmit(char val) {

	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = val;
	
}

void USART_Transmit_String(char *val) {
	while(*val != NULL) {
		USART_Transmit(*val);
		val++;
	}
}

void echo() {
	USART_Transmit_String(USARTReadBufferArr);
	USART_Transmit_String("\n");
}

void clear_pin_to_check_level_one() {
	pin_to_check_level_one[0] = NULL;
	pin_to_check_level_one[1] = NULL;
}
