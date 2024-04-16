/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Dat Vo
* Student ID:: 922171975
* Github-Name:: datvosfsu
* Project:: Assignment 2 - Tapeless Ruler
*
* File:: assignment2.h
*
* Description:: A program that controls a single AC motor
* through I2C communication between the raspberry pi
* and a motor hat connected to the pi. Motor configurations
* are sent from the program to the PCA9685 module on the hat.
* Motor functions are initiated by a button press, which is 
* received using the pigpio library. Once, the button is pressed,
* the motor will start at full speed, gruadually decrease to 
* 15%, and then stop for a full second. Finally, it will gradually 
* speed back up to max speed before the program fully finishes.
**************************************************************/

#ifndef __ASSIGNMENT3_H__
#define __ASSIGNMENT3_H__

#include "PCA9685.h"
#include "DEV_Config.h"
#include <pigpio.h>
#include <pthread.h>
#include <signal.h>

#define MAX_SPEED   70             // max motor duty cycle
#define PWMA        PCA_CHANNEL_0   // PCA channel(PWMA) duty cycle 
#define PWMB        PCA_CHANNEL_5
#define AIN1        PCA_CHANNEL_1   // PCA channel(AIN1) for Motor A backwards
#define AIN2        PCA_CHANNEL_2   // PCA channel(AIN2) for Motor A forward
#define BIN1        PCA_CHANNEL_3   // PCA channel(BIN1) for Motor A backwards
#define BIN2        PCA_CHANNEL_4   // PCA channel(BIN2) for Motor A forward
#define TARGET      0x42            // I2C target address
#define TARGET2     0x49            // I2C target address

#define IR_OUT 14           // gpio pin for IR sensor 
// #define LINE_OUT 15         // gpio pin for line sensor




#endif