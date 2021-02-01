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


#define Sample_Frequency 2000
#define SPI0_BaudRate 500000
#define MANUFACTURE_ID 0xef3011
#define PAGE_LEN 256


#define BP1 LPC_GPIO_PORT->B0[13]
#define BP2 LPC_GPIO_PORT->B0[12]
#define LED2 LPC_GPIO_PORT->B0[17]
#define LED3 LPC_GPIO_PORT->B0[21]
#define LED4 LPC_GPIO_PORT->B0[11]


uint8_t buffer_0[PAGE_LEN] = {0};
uint8_t buffer_1[PAGE_LEN] = {0};
uint32_t page_number = 0;
uint8_t buffer_full_flag = 0;
uint8_t buffer_empty_flag = 0;
uint8_t flash_full_flag = 0;
uint8_t etat = 0; //0 stand by, 1 read FLASH, 2 write on flash
uint8_t buffer_select = 0;
uint16_t buffer_index = 0;
uint16_t last_page = 0;




void init_SysTick()
{
	//mise en place de l'horloge d'échantillonnage cadencé par SysTick
	SysTick->LOAD = 15000000/Sample_Frequency;
	// clock = system_clock, tick interrupt enabled, count enabled
	SysTick->CTRL = (1<<SysTick_CTRL_CLKSOURCE_Pos) | (1<<SysTick_CTRL_TICKINT_Pos);
	NVIC_EnableIRQ(SysTick_IRQn);
}


void send_byte(uint8_t b)
{
	LPC_USART0->TXDAT = b;
}

void lancer_lecture()
{
	buffer_index = 0;
	buffer_select = 0;
	LPC_USART0->CFG |= UART_EN;
	last_page = 512;
	page_number = 0;
	//lancer l'horloge
	SysTick->VAL = 0;
	SysTick->CTRL |= (1<<SysTick_CTRL_ENABLE_Pos);
}


void SysTick_Handler(void)
{
	// Clear the interrupt flag by reading the SysTick_CTRL register
	uint32_t temp = SysTick->CTRL;

	if(buffer_index >= PAGE_LEN)
	{
		buffer_empty_flag = 1;
		buffer_index = 0;
		if(buffer_select == 0)
		  buffer_select = 1;
		else
		  buffer_select = 0;

		if(page_number == last_page)
		{
			SysTick->CTRL &= ~(1<<SysTick_CTRL_ENABLE_Pos);
		}
	}
	else
	{
		if(buffer_select == 0)
		{
			send_byte(buffer_0[buffer_index++]);
		}
		else
		{
			send_byte(buffer_1[buffer_index++]);
		}
	}
}



int main(void)
{
	static int i = 0 ;
	uint32_t gFlashID = 0;

	//set main clock
 	LPC_PWRD_API->set_fro_frequency(30000);

 	//ADC and DAC on
 	LPC_SYSCON->PDRUNCFG &= ~(1<<4) & ~(1<<13);

 	//clock enable
	LPC_SYSCON->SYSAHBCLKCTRL0 |= ( SWM | GPIO | SPI0 | CTIMER0 | IOCON | ADC | DAC0 | UART0);

	LPC_GPIO_PORT->DIR0 |= (1<<17)|(1<<21)|(1<<11);
	LED2 = 0;
	LED3 = 0;
	LED4 = 0;


	//Config USART
	ConfigSWM(U0_TXD, DBGTXPIN);
	ConfigSWM(U0_RXD, DBGRXPIN);

	// on définit l'horloge de l'uart sur la main clock
	LPC_SYSCON->UART0CLKSEL = FCLKSEL_MAIN_CLK;

	//clear usart
	LPC_SYSCON->PRESETCTRL0 &= UART0_RST_N;
	LPC_SYSCON->PRESETCTRL0 |= ~(UART0_RST_N);

	//configure débit
	LPC_USART0->BRG = 9;
	LPC_USART0->OSR = 12;

	//config communication
	LPC_USART0->CFG = DATA_LENG_8 | PARITY_NONE | STOP_BIT_1;
	LPC_USART0->CTL = 0;

	  /*=========================*/
	 /* CONFIGURE SPI FOR FLASH */
	/*=========================*/

	//clear SPI0
	LPC_SYSCON->PRESETCTRL0 &= (SPI0_RST_N);
	LPC_SYSCON->PRESETCTRL0 |= ~(SPI0_RST_N);

	//source clock for SPI0
	LPC_SYSCON->SPI0CLKSEL = FCLKSEL_MAIN_CLK;

	//SWM pin configuration
	ConfigSWM(SPI0_SCK,   P0_1); //SCK : clock signal
	ConfigSWM(SPI0_MISO,  P0_9); //MISO : Master In Slave Out : data slave to master
	ConfigSWM(SPI0_MOSI,  P0_8); //MOSI : Master Out Slave In : data master to slave

	SPI_FLASH_CS1(); /* Make the port bit an output driving '1' */
	LPC_GPIO_PORT->DIRSET[0] = 1<<P0_16;

	//clock 1.6 MHz
	LPC_SPI0->DIV = (15000000/SPI0_BaudRate) - 1;

	//enable SPI in Master state
	LPC_SPI0->CFG = SPI_CFG_ENABLE | SPI_CFG_MASTER;

	//no delays
	LPC_SPI0->DLY = 0x0000;

	gFlashID = spiflash_read_id();
	if(gFlashID != MANUFACTURE_ID)
	{
		while(1);
	}

	// Set WEL bit to enable flash write
	spiflash_write_enable();
	while ( (spiflash_check_status()&SPIFLASH_Status_Busy) != 0 );

	// WEL will be gone after erase, check status register and make sure it's set before programming.
	while ( (spiflash_check_status()&SPIFLASH_Status_WEL) == 0 ) {
		spiflash_write_enable();
	}



	  /*================*/
	 /* Initialization */
	/*================*/

	//clock
	init_SysTick();

	NVIC->IP[4] = 0;

	uint8_t bp2 = 1;
	uint8_t bp2_old = 1;
	uint8_t bp1 = 1;
	uint8_t bp1_old = 1;

	init_lcd();
	lcd_position(0,0);
	lcd_puts("Post-it vocal");

    while(1)
    {
    	//Button 1
    	bp1 = BP1;
    	if(bp1 == 0 && bp1_old == 1) //BP1 pushed
		{
    		bp1_old = bp1;
    		lancer_lecture();
		}
		else if(bp1 == 1 && bp1_old == 0) //BP1 released
		{
			bp1_old = bp1;
		}

    	if(buffer_empty_flag)
		{
			buffer_empty_flag = 0;
			while ( (spiflash_check_status()&SPIFLASH_Status_Busy) != 0 );
			if(buffer_select == 0)
			{
				spiflash_read_data(page_number++*PAGE_LEN, PAGE_LEN, buffer_1);
			}
			else
			{
				spiflash_read_data(page_number++*PAGE_LEN, PAGE_LEN, buffer_0);
			}
		}

    	i++;
    }
    return 0;
}
