#ifndef __CARFUNCTIONS__
#define __CARFUNCTIONS__

#include "PCA9685.h"
#include "DEV_Config.h"
#include <pigpio.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_RIGHT_SPEED     70              // max motor duty cycle
#define MAX_LEFT_SPEED      63              // max motor duty cycle
#define PWMA                PCA_CHANNEL_0   // PCA channel(PWMA) duty cycle 
#define PWMB                PCA_CHANNEL_5
#define AIN1                PCA_CHANNEL_1   // PCA channel(AIN1) for Motor A backwards
#define AIN2                PCA_CHANNEL_2   // PCA channel(AIN2) for Motor A forward
#define BIN1                PCA_CHANNEL_3   // PCA channel(BIN1) for Motor A backwards
#define BIN2                PCA_CHANNEL_4   // PCA channel(BIN2) for Motor A forward
#define LEFT_SIDE           0x49            // I2C target address
#define RIGHT_SIDE          0x40            // I2C target address

#define IR_OUT              14              // gpio pin for IR sensor 
#define FRONT_RIGHT_IR_OUT  18              // gpio pin for left back IR sensor
#define TRIGGER             23              // gpio pin for left front IR sensor 
#define ECHO                24              // gpio pin for left front IR sensor
#define LINE_RIGHT_OUT      15              // gpio pin for line sensor
#define LINE_LEFT_OUT       17              // gpio pin for line sensor
#define VELOCITY            0.034           // cm/uS
#define BUTTON              27              // gpio pin for button input

#endif