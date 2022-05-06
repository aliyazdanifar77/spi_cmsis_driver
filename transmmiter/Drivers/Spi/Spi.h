/*
 * Spi.h
 *
 *  Created on: May 6, 2022
 *      Author: Ali Yazdanifar
 */

#ifndef SPI_SPI_H_
#define SPI_SPI_H_

#include "stdint.h"
#include "main.h"


#define master 1
#define slave  0

void Spi1_Init(uint8_t state);
void Spi1_GpioInit(uint8_t state);
void Spi1_Enabling(uint8_t state);
void Spi1_Disabling(uint8_t state);
void Spi1_SendData(uint8_t *data , uint32_t len);
void Spi1_ReciveData(uint8_t *data ,uint8_t len);


#endif /* SPI_SPI_H_ */
