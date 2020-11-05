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



char *LEVEL_ONE_PIN = "123";
//char LEVEL_ONE_PIN[3] = {'1','2','3'};
// char *ACTION_MOVE = "move";
// char *ACTION_PIN = "pin1";

char *ACTION_MOVE = "mov";
char *ACTION_PIN = "pin";

char *MOVE_RIGHT = "-->";
char *MOVE_LEFT = "<--";


int usart_read_index = 0;
int usart_read_index_action = 0;
int usart_read_index_move = 0;
char pin_to_check_level_one[3] = {};
char where_to_go[3] = {};
char action_to_check[3] = {};

int global_pincheck = 55;
int global_move_check = 29;

int incorrect_pin_counters = 0;
enum INPUT {pin_correct, pin_incorrect, waiting};
enum INPUT input_action = waiting;

enum ACTION {pin, move, wait};
enum ACTION to_do = wait;

enum MOVE {forward, back, stay};
enum MOVE go_there = stay;

enum STATE {state1, state2, state3, state4, state5,state6};
enum STATE next_state = state1;

void checkAction();
void checkInput();
void checkState();

void echo();

void clear_pin_to_check_level_one();
void clear_action_to_check();
void clear_move_to_check();

int main(void){
	USARTInit();
	
	PortInit();

	sei ();
	clear_pin_to_check_level_one();
	clear_move_to_check();
	clear_action_to_check();
	while (1) {
		//checkAction();
		checkInput();
		checkState();
	}

}

void checkState() {
	switch(next_state) {
		
		
		case state1:
		USART_Transmit_String("current state 1");
		if (incorrect_pin_counters < 3) {
			next_state = state2;
			input_action = waiting;
			USART_Transmit_String("next state 2");
		}
		else {
			next_state = state4;
			input_action = waiting;
			USART_Transmit_String("next state 4");
		}
		break;
		
		case state2:
		to_do = pin;
		USART_Transmit_String("current state 2");
		if (input_action == pin_correct) {
			next_state = state3;
			input_action = waiting;
			incorrect_pin_counters = 0;
			USART_Transmit_String("next state 3");
			
		}
		else if (input_action == pin_incorrect) {
			next_state = state1;
			input_action = waiting;
			incorrect_pin_counters++;
			USART_Transmit_String("next state 1");
			
		}
		break;
		
		case state3:
		to_do = move;
		USART_Transmit_String("current state 3");
		if (go_there == forward) {
			next_state = state6;
			input_action = waiting;
			go_there = stay;
			USART_Transmit_String("next state 6");
			
		}
		else if (go_there == back) {
			next_state = state1;
			input_action = waiting;
			go_there = stay;
			USART_Transmit_String("next state 1");
		}
		
		break;
		
		case state4:
		to_do = pin;
		incorrect_pin_counters = 0;
		USART_Transmit_String("current state 4");
		if (input_action == pin_correct) {
			next_state = state1;
			input_action = waiting;
			USART_Transmit_String("next state 1");
			
		}
		else if (input_action == pin_incorrect) {
			next_state = state5;
			input_action = waiting;
			USART_Transmit_String("next state 5");
			
		}
		break;
		
		
		case state5:
		to_do = wait;
		USART_Transmit_String("current state 5");
		
		next_state = state5;
		input_action = waiting;
		USART_Transmit_String("ALARM! LED0");
		
		break;
		
		case state6:
		to_do = pin;
		USART_Transmit_String("current state 6");
		if (pin == pin_correct) {
			next_state = state6;
			input_action = waiting;
			USART_Transmit_String("next state 7; TODO");
			
		}
		else if (pin == pin_correct) {
			next_state = state6;
			input_action = waiting;
			USART_Transmit_String("next state 9; TO DO");
		}
		
		break;
		
		
	}

}


void checkAction() {
	
	int testr = 11000;
	testr = strcmp(action_to_check, ACTION_PIN);
	
	if (testr == 0 && to_do != pin) {
		USART_Transmit_String("WAITING FOR PIN\n");
		clear_pin_to_check_level_one();
		clear_move_to_check();
		
		usart_read_index = 0;
		usart_read_index_move = 0;
		usart_read_index_action = 0;
		to_do = pin;
	}
	
	testr = strcmp(action_to_check, ACTION_MOVE);
	if (testr == 0 && to_do != move) {
		USART_Transmit_String("WAITING FOR MOVE TODO\n");
		clear_action_to_check();
		
		usart_read_index = 0;
		usart_read_index_move = 0;
		usart_read_index_action = 0;
		to_do = move;
		
	}
	clear_action_to_check();
	//clear_move_to_check();
	
	
	usart_read_index = 0;
	usart_read_index_move = 0;
	usart_read_index_action = 0;
}

void checkInput() {
	int result = 11000;
	global_pincheck = strcmp(pin_to_check_level_one,LEVEL_ONE_PIN);
	
	switch(to_do) {
		case pin:
		
		
		USART_Transmit_String("PIN : ");
		USART_Transmit_String(pin_to_check_level_one);
		USART_Transmit_String("\n");

		if (global_pincheck == 0) {
			input_action = pin_correct;
			clear_pin_to_check_level_one();
			clear_move_to_check();
			usart_read_index_action = 0;
			
			} else if (global_pincheck != 0 && pin_to_check_level_one[0] != NULL && pin_to_check_level_one[1] != NULL) {
			input_action = pin_incorrect;
			clear_pin_to_check_level_one();
			clear_move_to_check();
			usart_read_index_action = 0;
		}
		else {
			input_action = waiting;
		}
		
		break;
		
		case move:
		USART_Transmit_String("MOVE : ");
		USART_Transmit_String(pin_to_check_level_one);
		USART_Transmit_String("\n");
		global_move_check = strcmp(pin_to_check_level_one, MOVE_RIGHT);
		
		
		if (global_move_check == 0 && go_there != forward) {
			USART_Transmit_String("MOVING FORWARD\n");
			clear_move_to_check();
			usart_read_index = 0;
			usart_read_index_move = 0;
			usart_read_index_action = 0;
			go_there = forward;
		} 
		
		global_move_check = strcmp(pin_to_check_level_one, MOVE_LEFT);
		if (global_move_check == 0 && go_there != back) {
			USART_Transmit_String("GOING BACK\n");
			usart_read_index = 0;
			usart_read_index_action = 0;
			usart_read_index_move = 0;
			go_there = back;
		}
		usart_read_index_move = 0;
		usart_read_index = 0;
		usart_read_index_action = 0;
 		clear_move_to_check();
// 		clear_action_to_check();
 		clear_pin_to_check_level_one();
		
		break;
		default:
		clear_move_to_check();
		clear_action_to_check();
		clear_pin_to_check_level_one();
		usart_read_index_action = 0;
		usart_read_index = 0;
		usart_read_index_move = 0;
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
	char bufferChar = NULL;
	bufferChar = UDR0;
	
	
	action_to_check[usart_read_index_action] = bufferChar;
	usart_read_index_action++;
	if (usart_read_index_action > 2) {
		usart_read_index_action = 0;
	}
	
	if (to_do == move) {
		pin_to_check_level_one[usart_read_index_move] = bufferChar;
		usart_read_index_move++;
		//usart_read_index_action = 0;
		usart_read_index = 0;
		if (usart_read_index_move > 2) {
			usart_read_index_move = 0;
			
		}
	}
	else if (to_do == pin) {
		pin_to_check_level_one[usart_read_index] = bufferChar;
		usart_read_index++;
		usart_read_index_move = 0;
		//usart_read_index_action = 0;
		if (usart_read_index > 2) {
			usart_read_index = 0;
			
		}
		
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

void clear_action_to_check() {
	action_to_check[0] = NULL;
}


void clear_move_to_check() {
	
	where_to_go[0] = NULL;
	where_to_go[1] = NULL;
}

