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

char *getsubstring();


void echo();

uint8_t USART_Read_Byte;
char USARTReadBufferArr[16] = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
uint8_t test_int = 0;
int usart_read_index = 0;
enum INPUT {correct, incorrect, waiting};
enum INPUT input_action = waiting;

char * LEVEL_ONE_PIN = "246367";
char pin_to_check_level_one[7] = {}; // note 6, not 5, there's one there for the null terminator

enum STATE {levelOneLocked, waitingLevel1Pin, verifyingLevelOnePin, locked, unlocked};
enum STATE next_state = levelOneLocked;
enum STATE prev_state = levelOneLocked;
enum STATE curr_state = levelOneLocked;

void checkState();
void checkInput();
void setToWait();

int main(void){
	USARTInit();
	
	PortInit();

	sei ();
	
	while (1) {
		//echo();
		//setToWait();
		checkInput();
		checkState();
		//setToWait();
	}

}

void checkState() {
	//USART_Transmit_String("check state");
	switch(next_state) {
		
		
		case locked:
		if (input_action == correct) {
			next_state = unlocked;
			input_action = waiting;
			
			USART_Transmit_String("A");
			USART_Transmit_String("current state: locked\n");
			USART_Transmit_String("next state: unlocked\n");
			USART_Transmit_String("unlocked - you can go\n");
		}
		else if (input_action == incorrect) {
			next_state = locked;
			input_action = waiting;
			
			USART_Transmit_String("B");
			USART_Transmit_String("current state: locked\n");
			USART_Transmit_String("next state: locked\n");
			USART_Transmit_String("locked - insert coin\n");
		}
		break;
		case unlocked:
		if (input_action == correct) {
			next_state = unlocked;
			input_action = waiting;
			
			USART_Transmit_String("C");
			USART_Transmit_String("current state: unlocked\n");
			USART_Transmit_String("next state: unlocked\n");
			USART_Transmit_String("unlocked already - you can go\n");
		}
		else if (input_action == incorrect) {
			next_state = locked;
			input_action = waiting;
			
			USART_Transmit_String("D");
			USART_Transmit_String("current state: unlocked\n");
			USART_Transmit_String("next state: locked\n");
			USART_Transmit_String("enjoy. locking after\n");
		}
		break;
		
	}/*
	switch(curr_state) {
		case levelOneLocked:
		USART_Transmit_String("Welcome to level 1. Level 1 locked, please enter 4 digit PIN:\n");
		next_state = levelOneLocked;
		curr_state = waitingLevel1Pin;
		input_action = waiting;
		break;
		
		case waitingLevel1Pin:
		if (input_action == correct) {
			USART_Transmit_String("PIN CORRECT!\n");
			next_state = waitingLevel1Pin;
			curr_state = waitingLevel1Pin;
		} else if (input_action == incorrect){
			USART_Transmit_String("PIN INCORRECT!\n");
			next_state = waitingLevel1Pin;
			curr_state = waitingLevel1Pin;
		}
		input_action = waiting;
		break;
	}
*/
}

void checkInput() {
	if (USART_Read_Byte[0] == 'c') {
		input_action = correct;
		
		} else if (USART_Read_Byte[0] == 'p') {
		input_action = incorrect;
		
	}
	else {
		input_action = waiting;
	}
	/*switch (curr_state) {
		case waitingLevel1Pin: 
		
		strncpy(pin_to_check_level_one, USARTReadBufferArr, 6);
		pin_to_check_level_one[7] = '\0'; // place the null terminator
		
		if (strcmp(pin_to_check_level_one, LEVEL_ONE_PIN) == 0) {
			input_action = correct;
			} else {
			//USART_Transmit_String("PIN INCORRECT!");
			input_action = incorrect;
		}
		break;
	}*/
}

void setToWait() {
	input_action = waiting;
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
	USARTReadBufferArr[usart_read_index] = USART_Read_Byte;

	usart_read_index++;
	if (usart_read_index > 15) { 
		usart_read_index = 0;
	}
}
/*^^-----------ISR----------------*/


void USART_Transmit(char val) {

	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = val;
	
}

void USART_Transmit_String(char *val) {
	if (input_action != waiting) {
		while(*val != NULL) {//'\0'
			USART_Transmit(*val);
			val++;
		}
	}
}


void echo() {
	USART_Transmit_String(USARTReadBufferArr);
	USART_Transmit_String("\n");
} 

/*
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
break;*/
