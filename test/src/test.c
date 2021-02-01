/*
===============================================================================
 Name        : post_it.c
 Author      : Thomas Mongaillard
 Version     : 1.0
 Copyright   : $(copyright)
 Description : vocal post-it using FLASH memory to store audio
===============================================================================
*/

/*
 * FONCTIONNEMENT
 * Un appui prolongé sur BP2 lance l'acquisition de l'audio par le module microphone + l'ADC.
 * On lit le signal audio enregistré en appuyant sur BP1.
 */

#include <cr_section_macros.h>
#include <stdio.h>
#include "LPC8xx.h"
#include "syscon.h"
#include "uart.h"
#include "rom_api.h"
#include "swm.h"
#include "lib_ENS_II1_lcd.h"
#include <string.h>
#include "chip_setup.h"
#include "core_cm0plus.h"
#include "utilities.h"
#include "spi.h"
#include "driver_spiflash.h"

#define SPI0_BaudRate 1600000
#define MANUFACTURE_ID 0xef3012


int main(void)
{
	static int i = 0 ;

	init_lcd();
	lcd_puts("                ");
	lcd_position(1,0);
	lcd_puts("                ");

    while(1)
    {
        i++;
    }

    return 0;
}
