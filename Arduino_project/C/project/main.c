#include "AVR_TTC_scheduler.h"
#include "USART.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//declareren variabelen.
uint16_t adc_result0, adc_result1;
/*int licht_voorkeur = 2;
int bool = 0;*/


// initiliseren van functies

void setup(void){
	DDRC = 0x00;
}

void adc_init()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	
	// ADC Enable and prescaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

// read adc value
uint16_t adc_read(uint8_t ch)
{
	// select the corresponding channel 0~7
	// ANDing with '7' will always keep the value
	// of 'ch' between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch;     // clears the bottom 3 bits before ORing
	
	// start single conversion
	// write '1' to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes '0' again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}

int licht_waarde(){
	int result;
	adc_result0 = adc_read(0);      // read adc value at PA0
	if (adc_result0 < 150)
	{
		result = 0;
		return result;
	}
	if (adc_result0 >150 && adc_result0 <300)
	{
		result = 1;
		return result;
	}
	if (adc_result0 >300 && adc_result0 <750)
	{
		result = 2;
		return result;
	}
	if (adc_result0 > 750)
	{
		result = 3;
		return result;
	}
	return result = 100;
}

int temp(){
	adc_result1 = adc_read(1);
	float voltage = adc_result1 * 5.0;
	voltage /= 1024.0;
	float temperatureC = (voltage - 0.5) * 10;
	int temp_int = temperatureC * 100;
	return temp_int;
}


void send_data(void){
	USART_putstring("x");
	UU_PutNumber(licht_waarde());
	UU_PutNumber(temp());
}

/*void lichtcheck(receive){
	//check wat er doorgestuurd wordt door python en zet daar de juiste led mee aan.
	if(receive == 0x6F)
	{
		naar_boven();
		bool == 0;
	}
	if(receive == 0x64)
	{
		naar_beneden();
		bool == 1;
	}
	if(receive == 0x73){
		PORTB = 0b00000000;
	}
}

void licht_voorkeur_check(receive){
	if (receive == 0x41 ){
		licht_voorkeur = 0;
	}
	if (receive == 0x42){
		licht_voorkeur = 1;
	}
	if (receive == 0x43 ){
		licht_voorkeur = 2;
	}
	if (receive == 0x44 ){
		licht_voorkeur = 3;
	}
}

void gebruikers_voorkeur(licht_voorkeur, licht_waarde ,bool){
	if (licht_voorkeur == 0 && licht_waarde ==0 && bool == 0)
	{
		naar_beneden();
	}
	if (licht_voorkeur == 1 && licht_waarde == 1&& bool == 0)
	{
		naar_beneden();
	}
	if (licht_voorkeur == 2 && licht_waarde == 2 && bool == 0)
	{
		naar_beneden();
	}
	if (licht_voorkeur == 3 && licht_waarde== 3 && bool == 0)
	{
		naar_beneden();
	}
	if(licht_waarde == 1 && bool == 1)
	{	
		naar_boven();
	}
}*/

int main()
{
	setup();
	led_init();
	adc_init();
	USART_init();
	
	SCH_Init_T1();
//	SCH_Add_Task(lichtcheck,0,10);
//	SCH_Add_Task(licht_voorkeur_check,0,10);
	SCH_Add_Task(temp,0,400);
	SCH_Add_Task(licht_waarde,0,300);
	SCH_Add_Task(send_data,0,600);
//	SCH_Add_Task(gebruikers_voorkeur,0,600);
	SCH_Add_Task(receive,0,10);
	
	
	SCH_Start();
	while(1){
		SCH_Dispatch_Tasks();
	}
	return 0;
}
