/*
 * dac.c
 *
 *  Created on: 2015Äê3ÔÂ25ÈÕ
 *      Author: zjm
 */

#include "stm32f10x.h"
#include <stdio.h>
#include "dac.h"



#define DAC_PORT	GPIOB

#define DAC_SPI_MOSI_PORT	DAC_PORT
#define DAC_SPI_MOSI_PIN	GPIO_Pin_13

#define DAC_SPI_MISO_PORT	DAC_PORT
#define DAC_SPI_MISO_PIN	GPIO_Pin_14

#define DAC_SPI_SCLK_PORT	DAC_PORT
#define DAC_SPI_SCLK_PIN	GPIO_Pin_15

#define DAC_SPI_CS_PORT		DAC_PORT
#define DAC_SPI_CS_PIN		GPIO_Pin_12

#define DAC_SPI_CS_CLR()	GPIO_ResetBits(DAC_SPI_CS_PORT,DAC_SPI_CS_PIN)
#define DAC_SPI_CS_SET()	GPIO_SetBits(DAC_SPI_CS_PORT,DAC_SPI_CS_PIN)

#define DAC_SPI_MOSI_CLR()	GPIO_ResetBits(DAC_SPI_MOSI_PORT,DAC_SPI_MOSI_PIN)
#define DAC_SPI_MOSI_SET()	GPIO_SetBits(DAC_SPI_MOSI_PORT,DAC_SPI_MOSI_PIN)

#define DAC_SPI_SCLK_CLR()	GPIO_ResetBits(DAC_SPI_SCLK_PORT,DAC_SPI_SCLK_PIN)
#define DAC_SPI_SCLK_SET()	GPIO_SetBits(DAC_SPI_SCLK_PORT,DAC_SPI_SCLK_PIN)

#define DAC_SPI_MISO()	((GPIO_ReadInputDataBit(DAC_SPI_MISO_PORT,DAC_SPI_MISO_PIN)==Bit_SET)?1:0)


#define NOP  (0x00)
#define Write_DAC_Data  (0x01)
#define Register_read   (0x02)
#define Write_control   (0x55)
#define Write_reset     (0x56)
#define Write_configuration (0x57)
#define Write_DAC_gain_calibration  (0x58)
#define Write_DAC_zero_calibration  (0x59)
#define Watchdog_timer_reset        (0x95)

#define delay()	do{\
	int ___j = 2;	\
	while(___j--);	\
}while(0)

struct _dac_reg regs[NUM_DAC];

void dac_init(void)
{
	int i;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = (DAC_SPI_MOSI_PIN) |
								  (DAC_SPI_SCLK_PIN) |
								  (DAC_SPI_CS_PIN) ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(DAC_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = (DAC_SPI_MISO_PIN);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DAC_PORT, &GPIO_InitStructure);

	DAC_SPI_CS_SET();

	for (i = 0; i < NUM_DAC; ++i) {
		regs[i].addr = Write_control;
		regs[i].reg = 0x1009;
	}
	for (i = 0; i < NUM_DAC; ++i) {
		dac_write(&regs);
	}

	for (i = 0; i < NUM_DAC; ++i) {
		regs[i].addr = Write_configuration;
		regs[i].reg = 0x0380;
	}
	dac_write(&regs);
}

uint8_t dac_read_write_byte(uint8_t c)
{
	int i;
	uint8_t res = 0;

	DAC_SPI_SCLK_CLR();

	for (i = 0; i < 8; ++i) {
		res <<= 1;
		if(c&0x80)
			DAC_SPI_MOSI_SET();
		else
			DAC_SPI_MOSI_CLR();
//		delay();
		DAC_SPI_SCLK_SET();
//		delay();
		if(DAC_SPI_MISO())
			res |= 1;
		DAC_SPI_SCLK_CLR();
//		delay();
		c <<= 1;
	}

	return res;
}



void dac_write(struct _dac_reg *conf)
{
	int i;

	DAC_SPI_CS_CLR();

	for (i = 0; i < NUM_DAC; ++i) {
		dac_read_write_byte(conf[i].addr);
		dac_read_write_byte(conf[i].reg>>8);
		dac_read_write_byte(conf[i].reg&0xFF);
	}

	DAC_SPI_CS_SET();
}

void dac_test(void)
{
	int i;
	for (i = 0; i < NUM_DAC; ++i) {
		regs[i].addr = Write_DAC_Data;
	}
	dac_write(&regs);
}

