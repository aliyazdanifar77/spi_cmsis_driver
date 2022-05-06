/*
 * Spi.c
 *
 *  Created on: May 6, 2022
 *      Author: Ali Yazdanifar
 *      it working with CMSIS Firmware
 *
 */


#include "Spi.h"



/////////////////////////////////////////////////////////////////
/*
SPI_CR1_BR			111
SPI_CR1_BR_0        001
SPI_CR1_BR_1        010
SPI_CR1_BR_2 		100
*/
/////////////////////////////////////////////////////////////////
/*
	Templet:
    00 - GPIOA->CRL &= ~GPIO_CRL_CNF0;
    11 - GPIOA->CRL |=  GPIO_CRL_CNF0;
    01 - GPIOA->CRL = (GPIOA->CRL & ~(GPIO_CRL_CNF0)) | GPIO_CRL_CNF0_0 ;
    10 - GPIOA->CRL = (GPIOA->CRL & ~(GPIO_CRL_CNF0)) | GPIO_CRL_CNF0_1 ;
*/
/////////////////////////////////////////////////////////////////
/*
  * for deiniting we use RCC to reset the peripheral
  *	RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
  *
*/
/////////////////////////////////////////////////////////////////
/*
 * 	initializing :
 * 	0- clock enabling (in APB2ENR or APB1ENR )
 *  1- master / slave (in control register (MSTR))
 *
 * 	2-1 full_duplex
 * 	2-2 half_duplex
 * 	2-3 simplex
 * 	in control reg in BIDIMODE(full,half) , BIDIMODE(trans , reci ) , RXONLY(simplex -force matster generating clock  )
 *
 * 	3- bundrate (form 0/256)
 * 	4- data format (8bit(defult) , 16bit) - (CR DFF)
 *  5- CPOL / CPHA
 *  6- SSM (software salve en/dis) / SSI (the value of NSS)
 *
*/
/////////////////////////////////////////////////////////////////
/*
 * 	send data :
 * 	1- send data to the TX_buffer
 * 	2- wait until TXE in status reg becomes 1
 * 	3- load DR(write it == write in Tx_buffer / read it == read from Rx_buffer) with data and increment buffer addr
 * 	3-1 before transmitting check :(TXE = 1 when TX_buffer empty)
 * 	3-2 before receiving check :(RXNE = 1 when RX_buffer not empty when we read it , it cleaed auto)
 * 	4- decrement data len
 * 	5- if len became 0 blocking function ended
 *
*/
/////////////////////////////////////////////////////////////////





void Spi1_Init(uint8_t state)
{

	/*	-- full duplex configuration --		*/

	if(state == master)
	{
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

		SPI1->CR1 |= SPI_CR1_MSTR;
		SPI1->CR1 &= ~SPI_CR1_BIDIMODE;
		SPI1->CR1 &= ~SPI_CR1_BR;
		SPI1->CR1 |=(SPI_CR1_BR_2 + SPI_CR1_BR_0);
		SPI1->CR1 &= ~(SPI_CR1_DFF);
		SPI1->CR1 &= ~SPI_CR1_CPOL;
		SPI1->CR1 &= ~SPI_CR1_CPHA;
		SPI1->CR1 &= ~SPI_CR1_SSM;
	}

	else
	{
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

		SPI1->CR1 &= ~SPI_CR1_MSTR;
		SPI1->CR1 &= ~SPI_CR1_BIDIMODE;
		SPI1->CR1 &= ~SPI_CR1_BR;
		SPI1->CR1 |=(SPI_CR1_BR_2 + SPI_CR1_BR_0);
		SPI1->CR1 &= ~(SPI_CR1_DFF);
		SPI1->CR1 &= ~SPI_CR1_CPOL;
		SPI1->CR1 &= ~SPI_CR1_CPHA;
		SPI1->CR1 &= ~SPI_CR1_SSM;
	}

	// be careful when SSM is enabled the SSI should be 1 if not , MODF become 1 and then SPE
	// become 0 (this error happen in multi master solution ) NSS = 0 this means that another master
	// taken over the bus.

}

void Spi1_GpioInit(uint8_t state)
{

	// pa7 mosi  Master: Alternate function push-pull / Slave: Input floating,Input pull-up
	// pa6 miso  Master:Input floating,Input pull-up  / Slave: Alternate function push-pull
	// pa5 sck   Master: Alternate function push-pull / Slave: Input floating
	// pa4 nss   Hardware master /slave: Input floating/ Input pull-up,Input pull-down

	// when we drive an external slave its better to enable pull up res for altfun pins
	if(state == master)
	{
		// for master

		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
		AFIO->MAPR &= ~(AFIO_MAPR_SPI1_REMAP);

		//configuring PA7:
		GPIOA->CRL &= ~(GPIO_CRL_CNF7 + GPIO_CRL_MODE7);
		GPIOA->CRL |= GPIO_CRL_MODE7;
		GPIOA->CRL |= GPIO_CRL_CNF7_1;

		//configuring PA6:
		GPIOA->CRL &= ~(GPIO_CRL_CNF6 + GPIO_CRL_MODE6);
		GPIOA->CRL |= (GPIO_CRL_CNF6_1);

		//configuring PA5:
		GPIOA->CRL &= ~(GPIO_CRL_CNF5 + GPIO_CRL_MODE5);
		GPIOA->CRL |= GPIO_CRL_MODE5;
		GPIOA->CRL |= GPIO_CRL_CNF5_1;
	}

	else
	{
		// for slave

		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
		AFIO->MAPR &= ~(AFIO_MAPR_SPI1_REMAP);

		//configuring PA7:
		GPIOA->CRL &= ~(GPIO_CRL_CNF7 + GPIO_CRL_MODE7);
		GPIOA->CRL |= GPIO_CRL_CNF7_1;

		//configuring PA6:
		GPIOA->CRL &= ~(GPIO_CRL_CNF6 + GPIO_CRL_MODE6);
		GPIOA->CRL |= GPIO_CRL_MODE6;
		GPIOA->CRL |= (GPIO_CRL_CNF6_1);

		//configuring PA5:
		GPIOA->CRL &= ~(GPIO_CRL_CNF5 + GPIO_CRL_MODE5);
		GPIOA->CRL |= GPIO_CRL_CNF5_1;

	}



}


void Spi1_SendData(uint8_t *data , uint32_t len)
{


	while(len > 0)
	{
		while(!(SPI1->SR & SPI_SR_TXE));
		SPI1->DR = *data;
		len--;
		data++;

	}

}

void Spi1_ReciveData(uint8_t *data ,uint8_t len)
{


	while(len > 0)
	{
		while(!(SPI1->SR & SPI_SR_RXNE));
		*data = SPI1->DR;
		len--;
		data++;
	}

}




void Spi1_Enabling(uint8_t state)
{
	/* when spi start working its parameters aren't
	 * changeable until the spi stop working with
	 * this codes and then start again
	*/

	if(state == 1)
		SPI1->CR1 |= SPI_CR1_SPE;
	else
		SPI1->CR1 &= ~SPI_CR1_SPE;
}


void Spi1_Disabling(uint8_t state)
{

while((SPI1->SR & SPI_SR_BSY));

SPI1->CR1 &= ~(SPI_CR1_SPE);

}




















