#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_
/***********************************************************************************************************************
			------------------------------------------------------------------------
			|\\\																///|
			|\\\					Hardware interface							///|
			------------------------------------------------------------------------
***********************************************************************************************************************/
#ifdef USE_BCM2835_LIB
    #include <bcm2835.h>
#endif

#include <stdint.h>
#include "Debug.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DEV_I2C 1
/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

extern int INT_PIN;//4
/*------------------------------------------------------------------------------------------------------*/
uint8_t DEV_ModuleInit(void);
void    DEV_ModuleExit(void);

void DEV_I2C_Init(uint8_t Add);
void I2C_Write_Byte(uint8_t Cmd, uint8_t value);
int I2C_Read_Byte(uint8_t Cmd);
int I2C_Read_Word(uint8_t Cmd);

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);
void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);
	
void DEV_Delay_ms(UDOUBLE xms);
#endif