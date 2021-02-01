/*
===============================================================================
 Name        : post_it.c
 Author      : Thomas Mongaillard
 Version     : 1.0
 Copyright   : $(copyright)
 Description : vocal post-it using FLASH memory to store audio
===============================================================================
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


#define Sample_Frequency 8000
#define SPI0_BaudRate 500000
#define MANUFACTURE_ID 0xef3011
#define PAGE_LEN 256


#define BP1 LPC_GPIO_PORT->B0[13]
#define BP2 LPC_GPIO_PORT->B0[12]
#define LED2 LPC_GPIO_PORT->B0[17]
#define LED3 LPC_GPIO_PORT->B0[21]
#define LED4 LPC_GPIO_PORT->B0[11]


uint8_t buffer_0[PAGE_LEN];
uint8_t buffer_1[PAGE_LEN];
uint16_t page_number = 0;
uint8_t state = 0; //0 stand by, 1 read FLASH, 2 write on flash
uint8_t buffer_full_flag = 0;
uint8_t buffer_empty_flag = 0;
uint8_t flash_full_flag = 0;
uint8_t buffer_select = 0;
uint16_t buffer_index = 0;
uint16_t last_page = 0;



void init_SysTick()
{
	SysTick->LOAD = 15000000/Sample_Frequency; //set frequency
	SysTick->CTRL = (1<<SysTick_CTRL_CLKSOURCE_Pos) | (1<<SysTick_CTRL_TICKINT_Pos);
	NVIC_EnableIRQ(SysTick_IRQn); //enable interruption
}

void start_acquisition(void)
{
	state = 2;

	//display
	lcd_position(0,0);
	lcd_puts("Enregistrement..");
	lcd_position(1,0);
	lcd_puts("Attente");

	//chip erase
	spiflash_write_enable();
	spiflash_chip_erase();
	while ( (spiflash_check_status()&SPIFLASH_Status_Busy) != 0 );

	//initialization
	page_number = 0;
	buffer_index = 0;
	buffer_select = 0;

	//clear
	lcd_position(1,0);
	lcd_puts("                ");

	//visualization
	LED2 = 1;

	//start interruptions
	SysTick->VAL = 0;
	SysTick->CTRL |= (1<<SysTick_CTRL_ENABLE_Pos);
}

void stop_acquisition(void)
{
	LED2 = 0; //visualization

	spiflash_write_disable();
	last_page = page_number; //save last page
	SysTick->CTRL &= ~(1<<SysTick_CTRL_ENABLE_Pos); //stop interruptions

	//display
	lcd_position(0,0);
	lcd_puts("                ");
	lcd_position(0,0);
	lcd_puts("Post-it vocal");
	lcd_position(1,0);
	lcd_puts("                ");
}



void start_reading_record(void)
{
	state = 1;

	//display
	lcd_position(0,0);
	lcd_puts("                ");
	lcd_position(0,0);
	lcd_puts("Lecture..");

	//visualization
	LED2 = 1;

	//init
	page_number = 0;
	buffer_index = 0;
	buffer_select = 1;

	//read the first page
	spiflash_read_data(page_number++*PAGE_LEN, PAGE_LEN, buffer_0);
	buffer_empty_flag = 1; //to start a new reading instruction

	//start interruptions
	SysTick->VAL = 0;
	SysTick->CTRL |= (1<<SysTick_CTRL_ENABLE_Pos);

	//LPC_IOCON->PIO0_19 |= 1<<16;
}



void stop_reading_record(void)
{
	state = 0;

	//LPC_IOCON->PIO0_19 &= ~(1<<16);

	LED2 = 0; //visualization


	LPC_DAC0->CR = 0; //reset DAC
	SysTick->CTRL &= ~(1<<SysTick_CTRL_ENABLE_Pos); //stop interruptions

	//display
	lcd_position(0,0);
	lcd_puts("                ");
	lcd_position(0,0);
	lcd_puts("Post-it vocal");
}

void data2dac(void)
{
	if(buffer_select == 0)
		LPC_DAC0->CR = buffer_0[buffer_index++]<<8; //conversion to 10 bits
	else
		LPC_DAC0->CR = buffer_1[buffer_index++]<<8;
}

void SysTick_Handler(void)
{
	// Clear the interrupt flag by reading the SysTick_CTRL register
	uint32_t temp = SysTick->CTRL;

	switch(state)
	{
		case 1: //reading record
			//buffer management
			if(buffer_index >= PAGE_LEN) //buffer filled
			{
				buffer_empty_flag = 1;
				buffer_index = 0;

				//change buffer
				if(buffer_select == 0)
				{
					buffer_select = 1;
				}
				else
				{
					buffer_select = 0;
				}

				//end of the record
				if(page_number == last_page)
				{
				  stop_reading_record();
				}
				else
					data2dac();
			}
			else //new value to DAC
				data2dac();
			break;

		case 2: //acquisition
		{
			//buffer management
			if(buffer_index >= PAGE_LEN) //buffer filled
			{
				buffer_full_flag = 1;
				buffer_index = 0;

				//change buffer
				if(buffer_select == 0)
				{
					buffer_select = 1;
				}
				else
				{
					buffer_select = 0;
				}

				//end of flash
				if(page_number == 512)
				{
					stop_acquisition();
					flash_full_flag = 1;
				}

				else
				{
					//start conversion on ADC
					LPC_ADC->SEQA_CTRL |= (1<<26);
				}
			}
			else
			{
				//Start conversion on ADC
				LPC_ADC->SEQA_CTRL |= (1<<26);
			}
			break;
		}
	}
}

void ADC_SEQA_IRQHandler(void)
{
	//read data from ADC
	uint8_t data = (LPC_ADC->SEQA_GDAT & 0xFFF0)>>8;
	if(buffer_select == 0)
		buffer_0[buffer_index++] = data;
	else
		buffer_1[buffer_index++] = data;
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
	LPC_SYSCON->SYSAHBCLKCTRL0 |= ( SWM | GPIO | SPI0 | IOCON | ADC | DAC0);

	LPC_GPIO_PORT->DIR0 |= (1<<17)|(1<<21)|(1<<11);
	LED2 = 0;
	LED3 = 0;
	LED4 = 0;



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
	//spiflash_write_enable();
	//spiflash_chip_erase();
	//while ( (spiflash_check_status()&SPIFLASH_Status_Busy) != 0 );

	  /*===============*/
	 /* CONFIGURE ADC */
	/*===============*/

	LPC_SYSCON->ADCCLKSEL = 0;
	LPC_SYSCON->ADCCLKDIV = 1;

	LPC_ADC->CTRL = 15; //100 kHz

	//ADC8 PIN 15
	LPC_SWM->PINENABLE0 &= ~(1<<18);
	LPC_IOCON->PIO0_15 &= ~(0b11<<3);


	//configure channel 8
	LPC_ADC->SEQA_CTRL |= (1<<8);
	LPC_ADC->SEQA_CTRL |= (1<<31);

	//enable interrupt at each conversion completed
	LPC_ADC->INTEN = 1;

	NVIC_EnableIRQ(ADC_SEQA_IRQn);


	  /*===============*/
	 /* CONFIGURE DAC */
	/*===============*/

	//pin 19 en mode sortie analogique
	LPC_SWM->PINENABLE0 &= ~(1<<23);
	LPC_IOCON->PIO0_19 |= 1<<16;



	  /*================*/
	 /* Initialization */
	/*================*/

	//clockQ
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
    		if(state == 0)
    		{
    			start_reading_record();
    		}
		}
		else if(bp1 == 1 && bp1_old == 0) //BP1 released
		{
			bp1_old = bp1;
		}


    	//Button 2
    	bp2 = BP2;
    	if(bp2 == 0 && bp2_old == 1) //BP2 pushed
    	{
    		bp2_old = bp2;
    		if(state == 0)
    			start_acquisition();
    	}
    	else if(bp2 == 1 && bp2_old == 0) //BP2 released
    	{
    		bp2_old = bp2;
    		if(state == 2)
    		{
    			// flash memory full - stop_acquisition() already done
    			if(flash_full_flag)
    			{
    				state = 0;
    				flash_full_flag = 0;
    			}
    			else
    			{
    				stop_acquisition();
    				state = 0;
    			}
    		}

    	}

    	//write full buffer
    	if(buffer_full_flag)
    	{
    		while ( (spiflash_check_status()&SPIFLASH_Status_WEL) == 0 ) {
    				spiflash_write_enable();
    		}
    		buffer_full_flag = 0;
    		if(buffer_select == 0)
    		{
    			spiflash_write_data(page_number++*PAGE_LEN, PAGE_LEN, buffer_1);
    		}
    		else
    		{
    			spiflash_write_data(page_number++*PAGE_LEN, PAGE_LEN, buffer_0);
    		}
    	}

    	//read new page
    	if(buffer_empty_flag)
    	{
    		buffer_empty_flag = 0;
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
