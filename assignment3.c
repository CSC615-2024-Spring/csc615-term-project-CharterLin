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

volatile int irValue = 1;       // input value from ir sensor
volatile int lineValue = 1;     // input value from line sensor
volatile int keepLooping = 1;   // tells program when to finish and exit

typedef struct threadInfo
{
    pthread_t id;               // thread id returned from thread creation
    unsigned int pinNumber;     // pin that the thread will read from
    volatile int* valuePointer; // pointer to the integer value that the thread will modify
} threadInfo;

threadInfo* irThread;
threadInfo* lineThread1;
threadInfo* lineThread2;

void initialize()
{
    DEV_ModuleInit();
    PCA9685_Init(TARGET);
    if(gpioInitialise() < 0) 
    {
        printf("pigio initialization failed\n");
    }

    gpioSetMode(IR_OUT, PI_INPUT);
    irThread = malloc(sizeof(threadInfo));
    irThread->pinNumber = IR_OUT;       // this thread will run for ir sensor 
    irThread->valuePointer = &irValue;  // this thread will modify ir readings
}

// thread routine: reads pin value from given pin number and transfer that value to
// given integer variable
void *readSensor(void* param)
{
    threadInfo *thread = (threadInfo*)param;
    while(1)
    {
        if(!keepLooping)
            pthread_exit((void*)0);

        // update old variable values with new sensor readings
        *thread->valuePointer = gpioRead(thread->pinNumber);   
    }
}

int createThreads()
{
    if(pthread_create(&irThread->id, NULL, &readSensor, (void*) irThread) != 0)
    // || pthread_create(&lineThread.id, NULL, &readSensor, (void*) &lineThread) != 0)
    {
        perror("Thread creation error");
        return 1;
    }
}

// Ctrl + C signal handler. Signals the program to cleanup threads and close down.
static void endProgram()
{
    keepLooping = 0;
}

int main(void)
{   
    initialize();
    createThreads();
    
    PCA9685_SetPWMFreq(100, TARGET);
    
    PCA9685_SetLevel(AIN1, 0, TARGET);          
    PCA9685_SetLevel(AIN2, 1, TARGET);          
    PCA9685_SetLevel(BIN1, 0, TARGET);         
    PCA9685_SetLevel(BIN2, 1, TARGET);          

    PCA9685_SetLevel(AIN1, 0, TARGET2);          
    PCA9685_SetLevel(AIN2, 1, TARGET2);          
    PCA9685_SetLevel(BIN1, 0, TARGET2);         
    PCA9685_SetLevel(BIN2, 1, TARGET2);          

    PCA9685_SetPwmDutyCycle(PWMA, MAX_SPEED, TARGET);
    PCA9685_SetPwmDutyCycle(PWMB, MAX_SPEED, TARGET);

    PCA9685_SetPwmDutyCycle(PWMA, MAX_SPEED, TARGET2);
    PCA9685_SetPwmDutyCycle(PWMB, MAX_SPEED, TARGET2);

    signal(SIGINT, endProgram);     //set signal handler for ctrl+c
    while(keepLooping)
    {
        // checks value obtained from ir sensor
        // and notify if an obstacle is detected 
        if(irValue == 0)
        {   
            printf("Obstacle detected, ");
            PCA9685_SetPwmDutyCycle(PWMA, 0, TARGET);
            PCA9685_SetPwmDutyCycle(PWMB, 0, TARGET);

            PCA9685_SetPwmDutyCycle(PWMA, 0, TARGET2);
            PCA9685_SetPwmDutyCycle(PWMB, 0, TARGET2); 
        }
        else
        {
            PCA9685_SetPwmDutyCycle(PWMA, MAX_SPEED, TARGET);
            PCA9685_SetPwmDutyCycle(PWMB, MAX_SPEED, TARGET);

            PCA9685_SetPwmDutyCycle(PWMA, MAX_SPEED, TARGET2);
            PCA9685_SetPwmDutyCycle(PWMB, MAX_SPEED, TARGET2);
            printf("All clear, ");
        }
        // checks value obtained from line sensor
        // and notify if sensor detects a line
        if(lineValue == 0)
            printf("on line\n");
        else
            printf("off line\n");

        usleep(500000);
    }
    // usleep(2000000);

    // // Duty cycle decrease by 5% every 0.3s, down to 15%
    // // Motor speed subsequently decreases down to 15%
    // for(int i = 5; (MAX_SPEED - i) >= 15; i += 5)
    // {
    //     PCA9685_SetPwmDutyCycle(PWMA, MAX_SPEED - i, TARGET);
    //     PCA9685_SetPwmDutyCycle(PWMB, MAX_SPEED - i, TARGET);

    //     PCA9685_SetPwmDutyCycle(PWMA, MAX_SPEED - i, TARGET2);
    //     PCA9685_SetPwmDutyCycle(PWMB, MAX_SPEED - i, TARGET2);
    //     usleep(300000);
    // }

    // Stop the motor, duty cycle 0%, for 1 second
    PCA9685_SetPwmDutyCycle(PWMA, 0, TARGET);
    PCA9685_SetPwmDutyCycle(PWMB, 0, TARGET);

    PCA9685_SetPwmDutyCycle(PWMA, 0, TARGET2);
    PCA9685_SetPwmDutyCycle(PWMB, 0, TARGET2);
    usleep(1000000);
    
    // Set motor to move backwards by alternating the channel
    // PCA9685_SetLevel(AIN1, 1);
    // PCA9685_SetLevel(AIN2, 0);
    // PCA9685_SetLevel(BIN1, 1);
    // PCA9685_SetLevel(BIN2, 0);

    // Restart the motor slowly, in reverse direction
    // Increase duty cycle by 5% every 0.3 second, until motor is at max speed
    // for(int i = 0; i <= MAX_SPEED; i += 5)
    // {
    //     PCA9685_SetPwmDutyCycle(PWMA, i);
    //     PCA9685_SetPwmDutyCycle(PWMB, i);
    //     usleep(300000);
    // }

    // Let motor run at max speed for 2s and then stop
    // usleep(2000000);
    // PCA9685_SetPwmDutyCycle(PWMA, 0);
    // PCA9685_SetPwmDutyCycle(PWMB, 0);

    //close libraries and free all mallocs
    gpioTerminate();
    DEV_ModuleExit();
    return 0;
}



// int main()
// {

//     gpioSetMode(LINE_OUT, PI_INPUT);
//     threadInfo lineThread;                 // create new struct to hold info for line thread
//     lineThread.pinNumber = LINE_OUT;       // this thread will run for line sensor
//     lineThread.valuePointer = &lineValue;  // this thread will modify line readings
        
//     // cleanup threads and exit program
//     if(pthread_join(irThread.id, NULL) || pthread_join(lineThread.id, NULL))
//     {
//         perror("\nProblem cleaning up threads.");
//         return 1;
//     }

//     printf("\nClosed program successfully.");
//     return 0;
    
// }



