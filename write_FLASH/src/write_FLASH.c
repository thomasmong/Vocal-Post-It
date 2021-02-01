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


#define SPI0_BaudRate 500000
#define MANUFACTURE_ID 0xef3011


#define BP1 LPC_GPIO_PORT->B0[13]
#define BP2 LPC_GPIO_PORT->B0[12]
#define LED2 LPC_GPIO_PORT->B0[17]

uint8_t buffer[64];
uint8_t buffer_read[64];


int main(void)
{
	static int i = 0 ;
	uint32_t gFlashID = 0;

	//set main clock
 	LPC_PWRD_API->set_fro_frequency(30000);

 	//ADC on
 	LPC_SYSCON->PDRUNCFG &= ~(1<<4);

 	//clock enable
	LPC_SYSCON->SYSAHBCLKCTRL0 |= (SWM | GPIO | SPI0 | CTIMER0 | IOCON);

	//LED2 en sortie
	LPC_GPIO_PORT->DIR0 |= (1<<17)|(1<<11)|(1<<21)|(1<<19);
	LED2 = 0;


	  /*=========================*/
	 /* CONFIGURE SPI FOR FLASH */
	/*=========================*/

	//clear SPI0
	LPC_SYSCON->PRESETCTRL0 &= (SPI0_RST_N);
	LPC_SYSCON->PRESETCTRL0 |= ~(SPI0_RST_N);

	//source clock for SPI0
	LPC_SYSCON->SPI0CLKSEL = FCLKSEL_MAIN_CLK;

	//SWM pin config
	ConfigSWM(SPI0_SCK,   P0_1); //SCK : clock signal
	ConfigSWM(SPI0_MISO,  P0_9); //MISO : Master In Slave Out : data slave to master
	ConfigSWM(SPI0_MOSI,  P0_8); //MOSI : Master Out Slave In : data master to slave

	SPI_FLASH_CS1(); /* Make the port bit an output driving '1' */
	LPC_GPIO_PORT->DIRSET[0] = 1<<P0_16;

	LPC_SPI0->DIV = (15000000/SPI0_BaudRate) - 1;

	//enable SPI in Master Mode
	LPC_SPI0->CFG = SPI_CFG_ENABLE | SPI_CFG_MASTER;

	//no delays
	LPC_SPI0->DLY = 0x0000;

	gFlashID = spiflash_read_id();
	if(gFlashID != MANUFACTURE_ID)
	{
		while(1);
	}

	spiflash_write_enable();
	// Set WEL bit to enable flash write
	spiflash_write_enable();
	spiflash_chip_erase();
	while ( (spiflash_check_status()&SPIFLASH_Status_Busy) != 0 );

	// WEL will be gone after erase, check status register and make sure it's set before programming.
	while ( (spiflash_check_status()&SPIFLASH_Status_WEL) == 0 ) {
		spiflash_write_enable();
	}


	init_lcd();
	lcd_position(0,0);
	lcd_puts("Post-it vocal");

	for(int k=0;k<64;k++)
	{
		buffer[k] = k%256;
	}

	uint8_t bp1 = 1;
	uint8_t bp1_old = 1;
	uint8_t test = 1;

    while(1)
    {
    	bp1 = BP1;
		if(bp1 == 0 && bp1_old == 1) //BP1 pushed
		{
			lcd_position(1,0);
			lcd_puts("                ");
			test = 1;
			bp1_old = bp1;
			LED2 = 1;
			spiflash_write_data(0x0, 64, buffer);
			LED2 = 0;

			while ( (spiflash_check_status()&SPIFLASH_Status_Busy) != 0 );
			LED2 = 1;
			spiflash_read_data(0x0, 64, buffer_read);
			LED2 = 0;
			for (int j = 4; j < 64; j++ )
			{
				if (buffer[j] != buffer_read[j] )
				{
					test = 0;
				}
			}
			if(test==1)
			{
				lcd_position(1,0);
				lcd_puts("Tout est bon");
			}
			else
			{
				lcd_position(1,0);
				lcd_puts("Erreur");
			}


		}
		else if(bp1 == 1 && bp1_old == 0) //BP1 released
		{
			bp1_old = bp1;
		}
    	i++;
    }
    return 0;
}
