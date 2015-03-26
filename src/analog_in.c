/*
 * analog_in.c
 *
 *  Created on: 2015Äê3ÔÂ25ÈÕ
 *      Author: zjm
 */

#include "stm32f10x.h"
#include <stdio.h>
#include "dac.h"

#define SPI_MISO_AI_PORT	GPIOA
#define SPI_MISO_AI_PIN		GPIO_Pin_6

#define SPI_MOSI_AI_PORT	GPIOA
#define SPI_MOSI_AI_PIN		GPIO_Pin_7

#define SPI_SCLK_AI_PORT	GPIOA
#define SPI_SCLK_AI_PIN		GPIO_Pin_5

#define SPI_CS_AI_0_PORT	GPIOA
#define SPI_CS_AI_0_PIN		GPIO_Pin_4

#define SPI_CS_AI_1_PORT	GPIOA
#define SPI_CS_AI_1_PIN		GPIO_Pin_3

#define SPI_CS_AI_2_PORT	GPIOA
#define SPI_CS_AI_2_PIN		GPIO_Pin_2

#define SPI_CS_AI_3_PORT	GPIOA
#define SPI_CS_AI_3_PIN		GPIO_Pin_1

#define SPI_AI_CS0_CLR()	GPIO_ResetBits(SPI_CS_AI_0_PORT,SPI_CS_AI_0_PIN)
#define SPI_AI_CS0_SET()	GPIO_SetBits(SPI_CS_AI_0_PORT,SPI_CS_AI_0_PIN)

#define SPI_AI_CS1_CLR()	GPIO_ResetBits(SPI_CS_AI_1_PORT,SPI_CS_AI_1_PIN)
#define SPI_AI_CS1_SET()	GPIO_SetBits(SPI_CS_AI_1_PORT,SPI_CS_AI_1_PIN)

#define SPI_AI_CS2_CLR()	GPIO_ResetBits(SPI_CS_AI_2_PORT,SPI_CS_AI_2_PIN)
#define SPI_AI_CS2_SET()	GPIO_SetBits(SPI_CS_AI_2_PORT,SPI_CS_AI_2_PIN)

#define SPI_AI_CS3_CLR()	GPIO_ResetBits(SPI_CS_AI_3_PORT,SPI_CS_AI_3_PIN)
#define SPI_AI_CS3_SET()	GPIO_SetBits(SPI_CS_AI_3_PORT,SPI_CS_AI_3_PIN)

#define SPI_AI_MOSI_CLR()	GPIO_ResetBits(SPI_MOSI_AI_PORT,SPI_MOSI_AI_PIN)
#define SPI_AI_MOSI_SET()	GPIO_SetBits(SPI_MOSI_AI_PORT,SPI_MOSI_AI_PIN)

#define SPI_AI_SCLK_CLR()	GPIO_ResetBits(SPI_SCLK_AI_PORT,SPI_SCLK_AI_PIN)
#define SPI_AI_SCLK_SET()	GPIO_SetBits(SPI_SCLK_AI_PORT,SPI_SCLK_AI_PIN)

#define SPI_AI_MISO_PIN()	((GPIO_ReadInputDataBit(SPI_MISO_AI_PORT,SPI_MISO_AI_PIN)==Bit_SET)?1:0)

#define delay()	do{\
	int ___j = 20;	\
	while(___j--);	\
}while(0)


extern struct _dac_reg regs[NUM_DAC];

void analog_in_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = (SPI_MOSI_AI_PIN) |
								  (SPI_SCLK_AI_PIN) |
								  (SPI_CS_AI_0_PIN) |
								  (SPI_CS_AI_1_PIN) |
								  (SPI_CS_AI_2_PIN) |
								  (SPI_CS_AI_3_PIN);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = (SPI_MISO_AI_PIN);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI_AI_CS0_SET();
	SPI_AI_CS1_SET();
	SPI_AI_CS2_SET();
	SPI_AI_CS3_SET();
}

uint8_t analog_in_read_write_byte(uint8_t c)
{
	int i;
	uint8_t res = 0;

	SPI_AI_SCLK_CLR();

	for (i = 0; i < 8; ++i) {
		res <<= 1;
		if(c&0x80)
			SPI_AI_MOSI_SET();
		else
			SPI_AI_MOSI_CLR();
		delay();
		SPI_AI_SCLK_SET();
		delay();
		SPI_AI_SCLK_CLR();
		delay();
		if(SPI_AI_MISO_PIN())
			res |= 1;
		c <<= 1;
	}

	return res;
}

#pragma pack(1)
struct _adc_data{
	uint8_t b55;
	uint8_t baa;
	uint16_t data[3];
}adc_buffer;
#pragma pack()

void analog_in_test(void)
{
	uint8_t *dp = (void *)&adc_buffer;
//	uint8_t dp[8];
	int i;

//	analog_in_init();

	SPI_AI_CS0_CLR();
	delay();
	for (i = 0; i < sizeof(struct _adc_data); ++i) {
		dp[i] = analog_in_read_write_byte(0xff);
		delay();
	}
	SPI_AI_CS0_SET();
	delay();
//	printf("channel 0 ad: %d\n",ad);

	regs[0].reg = adc_buffer.data[0];
	regs[1].reg = adc_buffer.data[1];

	SPI_AI_CS1_CLR();
	delay();
	for (i = 0; i < sizeof(struct _adc_data); ++i) {
		dp[i] = analog_in_read_write_byte(0xff);
		delay();
	}
	SPI_AI_CS1_SET();
	delay();
//	printf("channel 1 ad: %d\n",ad);

	regs[2].reg = adc_buffer.data[0];
	regs[3].reg = adc_buffer.data[1];

	SPI_AI_CS2_CLR();
	delay();
	for (i = 0; i < sizeof(struct _adc_data); ++i) {
		dp[i] = analog_in_read_write_byte(0xff);
		delay();
	};
	SPI_AI_CS2_SET();
	delay();
//	printf("channel 2 ad: %d\n",ad);

	regs[4].reg = adc_buffer.data[0];
	regs[5].reg = adc_buffer.data[1];

	SPI_AI_CS3_CLR();
	delay();
	for (i = 0; i < sizeof(struct _adc_data); ++i) {
		dp[i] = analog_in_read_write_byte(0xff);
		delay();
	}
	SPI_AI_CS3_SET();
	delay();
//	printf("channel 3 ad: %d\n",ad);
	regs[6].reg = adc_buffer.data[0];
	regs[7].reg = adc_buffer.data[1];
}

