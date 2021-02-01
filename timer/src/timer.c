/*
===============================================================================
 Name        : Telerupteur.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include <cr_section_macros.h>
#include <stdio.h>
#include "LPC8xx.h"
#include "syscon.h"
#include "lib_ENS_II1_lcd.h"



int main(void)
{
    // Force the counter to be placed into memory
	static int i = 0 ;

	//timer on
	LPC_SYSCON->SYSAHBCLKCTRL0 |= CTIMER0;

	//reset timer
	LPC_CTIMER0->TCR = 1;
	LPC_CTIMER0->TCR = 0;

	//set prescale
	LPC_CTIMER0->PR = 15000;


	init_lcd();

	//variable à afficher
	char value[17];

    // Enter an infinite loop, just incrementing a counter
    while(1)
    {
    	sprintf(value,"%s",LPC_CTIMER0->TC % 1000);
    	lcd_puts(value);
        i++ ;
    }

    return 0;
}
