#include "serial.h"
#include <avr/io.h>

#define FOSC 16000000  //Clock Frequency
#define BAUD 9600  //BAUD rate
#define MYUBRR FOSC/16/BAUD-1  // Value for UBBR0 register

unsigned char t[2];
extern volatile int bufcnt = 0;
extern volatile int checker = 0;
extern char buff[5];
extern volatile int myflag = 0;
extern volatile int myflag2 = 0;

void serial_stringout(char* s){
    // Call serial_txchar in loop to send a string
    int x = 0;
    while (s[x] != '\0'){
        serial_txchar(s[x]); //Sends each bit one at a time
        x++;
    }

}

void serial_txchar(char ch){
    while ((UCSR0A & (1 << UDRE0)) == 0);
    UDR0 = ch;
    
}

void serial_init(unsigned short ubrr_value){
    UBRR0 = ubrr_value;
    UCSR0B |= (1 << RXCIE0);
    // Set up USART0 registers
    UCSR0C = (3 << UCSZ00);
    UCSR0B |= (1 << TXEN0 | 1 << RXEN0);
    // Enable tri-state
    DDRD |= (1 << 3);
    //PORTD |= (1 << 3);
    PORTD &= ~(1 << 3);
}

ISR(USART_RX_vect){
        myflag = 1;
        char mychar;
        mychar = UDR0;

        //Check for valid start char
        if(mychar == '@'){
            myflag = 1;
            bufcnt = 0;

        }
        //Check for pos or neg temp
        else if(mychar == '+' || mychar == '-'){
            buff[bufcnt++] = mychar;
            if(bufcnt != 0){
                myflag = 0;
            }
        }
        //Add temp to buffer
        else if(myflag){
            if((mychar <= '9' && mychar >= '0') || mychar == '$'){
                if(mychar == '$'){
                    myflag2 = 1;
                    buff[bufcnt++] = '\0';
                    checker = 1;
                    bufcnt = 0;
                }
                else{
                    buff[bufcnt++] = mychar;
                }
            }
            else{
                myflag = 0;
            }
        }
    
}
