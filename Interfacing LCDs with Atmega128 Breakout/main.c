#include <stdbool.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include "lcd.h"


#define FIN1 PINC5
#define TRP1 PINE4
#define FOUT1 PINC4

volatile uint16_t ui16_counter = 0, ui16_lastCounter = 1;
volatile bool b_start = false, b_stop = false;
bool ui8_pressed = false;

ISR(TIMER1_OVF_vect)
{
	if(b_start)
	{
		ui16_counter++;
	}
	TCNT1H = 0xFF;
	TCNT1L = 0xF8;
	
}

/* Handler for PE4.*/
ISR(INT4_vect)
{
	//LCD_Clear();
	//LCD_Printf("Interrupt");
	if(b_start)
		b_stop = true;
}

int main(void)
{
	TCCR1A = 0x00;
	TCCR1B = 0x01;       // No prescale
	TCNT1H = 0xFF;
	TCNT1L = 0xF8; 	     //	1us = 65528 clock timer
	TIMSK  = 0x04;
	
	DDRC |= 1<<FOUT1;	 // Configure PC4 as output
	PORTC |= 1<<FOUT1;   // Default state FOUT is HIGH
	DDRC &= ~(1<<FIN1);  // Configure PC5 as input
	DDRE &= ~(1<<TRP1);  // Configure PE4 as input
	EICRB |= 0x02;		 // Failing edge
	EIMSK = (1<<TRP1);   // INT4
	sei();
	/*Connect RS->PB0, RW->PB1, EN->PB2 and data bus to PORTC.4 to PORTC.7*/
	LCD_SetUp(PB_0,PB_1,PB_2,P_NC,P_NC,P_NC,P_NC,PB_4,PB_5,PB_6,PB_7);
	LCD_Init(2,16);
	LCD_Printf("Project\n");
	LCD_Printf("ID: 30932094");
	
	while(1)
	{
		if ((!(PINC & (1<<FIN1)))&&(ui8_pressed == false))
		{
			PORTC &= ~(1<<FOUT1);	//Set FOUT1 = LOW
			b_start = true;
			ui8_pressed = true;
			//LCD_Clear();
			//LCD_Printf("PRESSED");
		}
		if ((PINC & (1<<FIN1))&&(ui8_pressed == true))
		{
			PORTC |= (1<<FOUT1);	//Set FOUT1 = HIGH
			ui8_pressed = false;
			//LCD_Clear();
			//LCD_Printf("RELEASED");
		}
		if((ui16_counter != ui16_lastCounter)&&(b_stop == false)&&(!(PINC & (1<<FIN1))))
		{
			LCD_Clear();
			LCD_Printf("%d us", ui16_counter);
		}
		if(b_stop == true)
		{
			LCD_Clear();
			LCD_Printf("Last %dus", ui16_counter);
			b_stop = false;
			b_start = false;
			ui16_counter = 0;
			ui16_lastCounter = 1;
		}
		ui16_lastCounter = ui16_counter;
		
	}
	return 0;
}


