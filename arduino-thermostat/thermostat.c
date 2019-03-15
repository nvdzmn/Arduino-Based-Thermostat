
#include "ds18b20.h"
#include "serial.h"
#include "lcd.h"
#include "encoder.h"
#include "thermostat.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

void setup_init(void);

int main(void){

    DDRC &= ~(1 << 2);
    DDRC &= ~(1 << 1);
    PORTC |= (1 << 1) | (1 << 2);

    DDRB |= ((1 << 3) | (1 << 4));
    //Initialize LCD
    lcd_init();
    lcd_writecommand(1);
    
    //Splash Screen
    lcd_moveto(0,0);
    lcd_stringout("EE109 Project");
    lcd_moveto(1,0);
    lcd_stringout("Navid Zaman");
    _delay_ms(1000);
    lcd_writecommand(1);

    //Initialization
    setup_init();

    //Global Variables
    short xTemp = 0;
    char buf[17];
    char buf2[17];

    while(1){
        //Starts temp conversion
        ds_temp(t);

        //Converts temp to fahrenheit
        xTemp = (t[0]);
        xTemp += (t[1]) * 256;
        xTemp = (((xTemp*9)+(32*80))/80);

        //Displays screen
        lcd_moveto(0,0);
        snprintf(buf, 17, "Temp:%d Rmt:", xTemp);
        lcd_stringout(buf);
        lcd_moveto(1,0);
        snprintf(buf2, 17, "High:%3d Low:%3d", countHigh, countLow);
        lcd_stringout(buf2);

        //Adjusts high temp threshold
        if((PINC & (1 << 1)) == 0){
            while((PINC & (1 << 1)) == 0){}
            button = 1;
            lcd_moveto(1,0);
            snprintf(buf2, 17, "High!%3d Low:%3d", countHigh, countLow);
            lcd_stringout(buf2);
        }
        //Adjusts low temp threshold
        if((PINC & (1 << 2)) == 0){
            while((PINC & (1 << 2)) == 0){}
            button = 2;
            lcd_moveto(1,0);
            snprintf(buf2, 17, "High:%3d Low!%3d", countHigh, countLow);
            lcd_stringout(buf2);
        }

        //Turns on AC (green LED)
        if(xTemp > countHigh){
            PORTB |= (1 << 4);
        }
        else {
            PORTB &= ~(1 << 4);
        }

        //Turns on heater (red LED)
        if(xTemp < countLow){
            PORTB |= (1 << 3);
        }
        else {
            PORTB &= ~(1 << 3);
        }

        //Generate proper temp serial
        char buftemp[6];
        buftemp[0] = '@';
        if(xTemp > 0){
            buftemp[1] = '+';
        }
        else{
            buftemp[1] = '-';
        }
        buftemp[2] = xTemp/10%10 + 48;
        buftemp[3] = xTemp%10 + 48;
        buftemp[4] = '$';
        buftemp[5] = '\0';
        serial_stringout(buftemp);

        //Recieve and display temp
        if (myflag == 1 && checker == 1){
            int recieved;
            sscanf(buff, "%d", &recieved);
            lcd_moveto(0, 13);
            char buf3[6];
            snprintf(buf3, 6, "%d", recieved);
            lcd_stringout(buf3);
            myflag = 0;
            checker = 0;
        }
    }

}

void setup_init(){
    //Initialize temp sensor
    ds_init();
    //Initialize rotary encoder
    enc_init();
    //Initialize serial
    serial_init(MYUBRR);
    //Enable global interrupts
    sei();
}







