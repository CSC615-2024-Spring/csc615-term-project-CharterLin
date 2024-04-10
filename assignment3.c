/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Dat Vo
* Student ID:: 922171975
* Github-Name:: datvosfsu
* Project:: Assignment 2 - Tapeless Ruler
*
* File:: assignment2.c
*
* Description:: A program that controls a single AC motor
* through I2C communication between the raspberry pi
* and a motor hat connected to the pi. Motor configurations
* are sent from the program to the PCA9685 module on the hat.
* Motor functions are initiated by a button press, which is 
* received using the pigpio library. Once the button is pressed,
* the motor will start at full speed, gradually decrease to 
* 15%, and then stop for a full second. Finally, it will gradually 
* speed back up to max speed before the program fully finishes.
**************************************************************/

#include "assignment3.h"

typedef struct threadInfo
{
    pthread_t id;               // thread id returned from thread creation
    int target;
} threadInfo;

void* routine(void* param) {
    threadInfo *thread = (threadInfo*)param;
    printf("in thread target %x\n", thread->target);
    PCA9685_Init(TARGET2);
    
    PCA9685_SetPWMFreq(100);
    
    PCA9685_SetLevel(AIN1, 0);          
    PCA9685_SetLevel(AIN2, 1);          
    PCA9685_SetLevel(BIN1, 0);         
    PCA9685_SetLevel(BIN2, 1);          

    PCA9685_SetPwmDutyCycle(PWMA, MAX_SPEED);
    PCA9685_SetPwmDutyCycle(PWMB, MAX_SPEED);
    usleep(2000000);

    // Duty cycle decrease by 5% every 0.3s, down to 15%
    // Motor speed subsequently decreases down to 15%
    for(int i = 5; (MAX_SPEED - i) >= 15; i += 5)
    {
        PCA9685_SetPwmDutyCycle(PWMA, MAX_SPEED - i);
        PCA9685_SetPwmDutyCycle(PWMB, MAX_SPEED - i);
        usleep(300000);
    }

    // Stop the motor, duty cycle 0%, for 1 second
    PCA9685_SetPwmDutyCycle(PWMA, 0);
    PCA9685_SetPwmDutyCycle(PWMB, 0);
    usleep(1000000);
    
    // Set motor to move backwards by alternating the channel
    PCA9685_SetLevel(AIN1, 1);
    PCA9685_SetLevel(AIN2, 0);
    PCA9685_SetLevel(BIN1, 1);
    PCA9685_SetLevel(BIN2, 0);

    // Restart the motor slowly, in reverse direction
    // Increase duty cycle by 5% every 0.3 second, until motor is at max speed
    for(int i = 0; i <= MAX_SPEED; i += 5)
    {
        PCA9685_SetPwmDutyCycle(PWMA, i);
        PCA9685_SetPwmDutyCycle(PWMB, i);
        usleep(300000);
    }

    // Let motor run at max speed for 2s and then stop
    usleep(2000000);
    PCA9685_SetPwmDutyCycle(PWMA, 0);
    PCA9685_SetPwmDutyCycle(PWMB, 0);

}

int main(void)
{   
    DEV_ModuleInit();

    threadInfo firstHAT;
    firstHAT.target = TARGET;

    threadInfo secondHAT;
    secondHAT.target = TARGET2;
    
    if(pthread_create(&firstHAT.id, NULL, &routine, (void*) &firstHAT) != 0)
    // || pthread_create(&secondHAT.id, NULL, &routine, (void*) &secondHAT) != 0)
    // pthread_create(&secondHAT.id, NULL, &routine, (void*) &secondHAT)
    // pthread_create(&firstHAT.id, NULL, &routine, (void*) &firstHAT)
    {
        perror("Thread creation error");
        return 1;
    }

    if(pthread_join(firstHAT.id, NULL) || pthread_join(secondHAT.id, NULL))
    {
        perror("\nProblem cleaning up threads.");
        return 1;
    }

    //close libraries
    DEV_ModuleExit();
    return 0;
}



