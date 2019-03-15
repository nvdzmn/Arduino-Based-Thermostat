
#include "encoder.h"


#include "serial.h"
#include <avr/interrupt.h>
#include <avr/eeprom.h>

volatile unsigned char new_state, old_state;	
volatile unsigned char a, b;
volatile int button;
volatile int countHigh, countLow;

void enc_init(){
	PORTC |= (1 << 3) | (1 << 4);  //Set up PB4 and PB5 as rotary inputs
  	PCICR |= (1 << PCIE1);  
  	PCMSK1 |= ((1 << PCINT11) | (1 << PCINT12)); 
	//sei();

	countLow = eeprom_read_byte((void*) 500);
	countHigh = eeprom_read_byte((void*) 501);
}

ISR (PCINT1_vect){

	unsigned char pin_input = PINC;
	a = (pin_input & (1 << 3));
	b = (pin_input & (1 << 4));  
	// For each state, examine the two input bits to see if state
	// has changed, and if so set "new_state" to the new state,
	// and adjust the count value.
	if(button == 1){	
		if (old_state == 0) {

			if (a != 0){   //Move to state 1 if a = 1 (CW)
				new_state = 1;
				countHigh++;
			}
			else if (b != 0) { // Move to state 3 if b = 1 (CCW)
				new_state = 3;
				countHigh--;
			}

		}
		else if (old_state == 1) {

	    	if (a == 0){    // Move to state 0 if a = 0 (CCW)
				new_state = 0;
				countHigh--;
			}
			else if (b != 0){   // Move to state 2 if b = 1 (CW)
				new_state = 2;
				countHigh++;
			}

		}
		else if (old_state == 2) {

	    	if (a == 0){   // Move to state 3 if a = 1 (CW)
				new_state = 3;
				countHigh++;
			}
			else if (b == 0){   // Move to state 0 if b = 0 (CCW)
				new_state = 0;
				countHigh--;
			}

		}
		else {   // old_state = 3

			if (a != 0){   // Move to state 2 if a = 0 (CCW)
				new_state = 2;
				countHigh--;
			}
			else if (b == 0){  // Move to state 0 if b = 0 (CW)
				new_state = 0;
				countHigh++;
			}    
		}
	// If state changed, update the value of old_state,
	// and set a flag that the state has changed.
		if (new_state != old_state) {
	    	old_state = new_state;
			eeprom_update_byte((void*)501, countHigh);
		}
		//High threshold cannot be greater than 100
		if(countHigh >= 100){
            countHigh = 100;
        }
		//If high threshold is less than low threshold, display as same
		if(countHigh < countLow){
			countHigh = countLow;
		}
	}
	if(button == 2){	
		if (old_state == 0) {

			if (a != 0){   //Move to state 1 if a = 1 (CW)
				new_state = 1;
				countLow++;
			}
			else if (b != 0) { // Move to state 3 if b = 1 (CCW)
				new_state = 3;
				countLow--;
			}

		}
		else if (old_state == 1) {

	    	if (a == 0){    // Move to state 0 if a = 0 (CCW)
				new_state = 0;
				countLow--;
			}
			else if (b != 0){   // Move to state 2 if b = 1 (CW)
				new_state = 2;
				countLow++;
			}

		}
		else if (old_state == 2) {

	    	if (a == 0){   // Move to state 3 if a = 1 (CW)
				new_state = 3;
				countLow++;
			}
			else if (b == 0){   // Move to state 0 if b = 0 (CCW)
				new_state = 0;
				countLow--;
			}

		}
		else {   // old_state = 3

			if (a != 0){   // Move to state 2 if a = 0 (CCW)
				new_state = 2;
				countLow--;
			}
			else if (b == 0){  // Move to state 0 if b = 0 (CW)
				new_state = 0;
				countLow++;
			}    
		}
	// If state changed, update the value of old_state,
	// and set a flag that the state has changed.
		if (new_state != old_state) {
	    	old_state = new_state;
			eeprom_update_byte((void*)500, countLow);
		}
		//Low threshold cannot be less than 40
		if(countLow <= 40){
            countLow = 40;
        }
		//If low threshold is greater than high threshold, display as same
		if(countLow > countHigh){
			countLow = countHigh;
		}
	}
	
}