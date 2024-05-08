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

volatile int echoDistance = 0;      // in cm
volatile int frontIrValue = 1;      // input value from ir sensor
volatile int lineRightValue = 0;       // input value from right line sensor
volatile int lineLeftValue = 0;     // input value from left line sensor
volatile int keepLooping = 1;       // tells program when to finish and exit

typedef struct threadInfo
{
    pthread_t id;               // thread id returned from thread creation
    unsigned int pinNumber;     // pin that the thread will read from
    volatile int* valuePointer; // pointer to the integer value that the thread will modify
} threadInfo;

threadInfo* frontIrThread;
threadInfo* lineRightThread;
threadInfo* lineLeftThread;
threadInfo* echoThread;

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

void *readEcho(void* param)
{
    threadInfo *thread = (threadInfo*)param;

    struct timeval start, end;
    while(1)
    {
        if(!keepLooping)
            pthread_exit((void*)0);

        gpioWrite(TRIGGER, 1);
        usleep(10);
        gpioWrite(TRIGGER, 0);

        while(gpioRead(ECHO) == 0) {}
        gettimeofday(&start, NULL);

        while(gpioRead(ECHO) == 1) {}
        gettimeofday(&end, NULL); 

        suseconds_t duration = end.tv_usec - start.tv_usec;

        *thread->valuePointer = ((int)duration * VELOCITY) / 2;
    }
}

int createThreads()
{
    if(pthread_create(&frontIrThread->id, NULL, &readSensor, (void*) frontIrThread) != 0
    || pthread_create(&lineRightThread->id, NULL, &readSensor, (void*) lineRightThread) != 0
    || pthread_create(&lineLeftThread->id, NULL, &readSensor, (void*) lineLeftThread) != 0
    || pthread_create(&echoThread->id, NULL, &readEcho, (void*) echoThread) != 0)
    {
        perror("Thread creation error");
        return 1;
    }

    return 0;
}

int initialize()
{
    DEV_ModuleInit();
    DEV_ModuleExit();
    DEV_ModuleInit();
    PCA9685_Init(LEFT_SIDE);
    PCA9685_Init(RIGHT_SIDE);
    if(gpioInitialise() < 0) 
    {
        printf("pigio initialization failed\n");
        return 1;
    }


    //TODO function to initialize these threads
    gpioSetMode(IR_OUT, PI_INPUT);
    frontIrThread = malloc(sizeof(threadInfo));
    frontIrThread->pinNumber = IR_OUT;       // this thread will run for ir sensor 
    frontIrThread->valuePointer = &frontIrValue;  // this thread will modify ir readings

    gpioSetMode(LINE_RIGHT_OUT, PI_INPUT);
    lineRightThread = malloc(sizeof(threadInfo));
    lineRightThread->pinNumber = LINE_RIGHT_OUT;       // this thread will run for ir sensor 
    lineRightThread->valuePointer = &lineRightValue;  // this thread will modify ir readings

    gpioSetMode(LINE_LEFT_OUT, PI_INPUT);
    lineLeftThread = malloc(sizeof(threadInfo));
    lineLeftThread->pinNumber = LINE_LEFT_OUT;       // this thread will run for ir sensor 
    lineLeftThread->valuePointer = &lineLeftValue;  // this thread will modify ir readings

    gpioSetMode(TRIGGER, PI_OUTPUT);
    gpioSetMode(ECHO, PI_INPUT);
    echoThread = malloc(sizeof(threadInfo));
    echoThread->pinNumber = ECHO;       // this thread will run for ir sensor 
    echoThread->valuePointer = &echoDistance;  // this thread will modify ir readings

    return createThreads();
}

// Ctrl + C signal handler. Signals the program to cleanup threads and close down.

void moveForward()
{
    PCA9685_SetLevel(AIN1, 0, LEFT_SIDE);          
    PCA9685_SetLevel(AIN2, 1, LEFT_SIDE);          
    PCA9685_SetLevel(BIN1, 0, LEFT_SIDE);         
    PCA9685_SetLevel(BIN2, 1, LEFT_SIDE);          

    PCA9685_SetLevel(AIN1, 1, RIGHT_SIDE);          
    PCA9685_SetLevel(AIN2, 0, RIGHT_SIDE);          
    PCA9685_SetLevel(BIN1, 1, RIGHT_SIDE);         
    PCA9685_SetLevel(BIN2, 0, RIGHT_SIDE);
}

void moveBackwards()
{
    PCA9685_SetLevel(AIN1, 1, LEFT_SIDE);          
    PCA9685_SetLevel(AIN2, 0, LEFT_SIDE);          
    PCA9685_SetLevel(BIN1, 1, LEFT_SIDE);         
    PCA9685_SetLevel(BIN2, 0, LEFT_SIDE);          

    PCA9685_SetLevel(AIN1, 0, RIGHT_SIDE);          
    PCA9685_SetLevel(AIN2, 1, RIGHT_SIDE);          
    PCA9685_SetLevel(BIN1, 0, RIGHT_SIDE);         
    PCA9685_SetLevel(BIN2, 1, RIGHT_SIDE);
}

void turnRight()
{
    PCA9685_SetLevel(AIN1, 0, LEFT_SIDE);          
    PCA9685_SetLevel(AIN2, 1, LEFT_SIDE);          
    PCA9685_SetLevel(BIN1, 0, LEFT_SIDE);         
    PCA9685_SetLevel(BIN2, 1, LEFT_SIDE);          

    PCA9685_SetLevel(AIN1, 0, RIGHT_SIDE);          
    PCA9685_SetLevel(AIN2, 1, RIGHT_SIDE);          
    PCA9685_SetLevel(BIN1, 0, RIGHT_SIDE);         
    PCA9685_SetLevel(BIN2, 1, RIGHT_SIDE);
}

void sharpTurnRight()
{
    PCA9685_SetLevel(AIN1, 0, LEFT_SIDE);          
    PCA9685_SetLevel(AIN2, 1, LEFT_SIDE);          
    PCA9685_SetLevel(BIN1, 0, LEFT_SIDE);         
    PCA9685_SetLevel(BIN2, 1, LEFT_SIDE);          

    PCA9685_SetPwmDutyCycle(PWMA, 0, RIGHT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, 0, RIGHT_SIDE);
}

void turnLeft()
{
    PCA9685_SetLevel(AIN1, 1, LEFT_SIDE);          
    PCA9685_SetLevel(AIN2, 0, LEFT_SIDE);          
    PCA9685_SetLevel(BIN1, 1, LEFT_SIDE);         
    PCA9685_SetLevel(BIN2, 0, LEFT_SIDE);          

    PCA9685_SetLevel(AIN1, 1, RIGHT_SIDE);          
    PCA9685_SetLevel(AIN2, 0, RIGHT_SIDE);          
    PCA9685_SetLevel(BIN1, 1, RIGHT_SIDE);         
    PCA9685_SetLevel(BIN2, 0, RIGHT_SIDE);
}

void sharpTurnLeft()
{
    PCA9685_SetPwmDutyCycle(PWMA, 0, LEFT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, 0, LEFT_SIDE);          

    PCA9685_SetLevel(AIN1, 1, RIGHT_SIDE);          
    PCA9685_SetLevel(AIN2, 0, RIGHT_SIDE);          
    PCA9685_SetLevel(BIN1, 1, RIGHT_SIDE);         
    PCA9685_SetLevel(BIN2, 0, RIGHT_SIDE);
}

void go()
{
    PCA9685_SetPwmDutyCycle(PWMA, MAX_LEFT_SPEED, LEFT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, MAX_LEFT_SPEED, LEFT_SIDE);

    PCA9685_SetPwmDutyCycle(PWMA, MAX_RIGHT_SPEED, RIGHT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, MAX_RIGHT_SPEED, RIGHT_SIDE);
}

void brake()
{
    PCA9685_SetPwmDutyCycle(PWMA, 0, LEFT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, 0, LEFT_SIDE);

    PCA9685_SetPwmDutyCycle(PWMA, 0, RIGHT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, 0, RIGHT_SIDE);
}

static void endProgram()
{   
    brake();
    gpioTerminate();
    DEV_ModuleExit();
    keepLooping = 0;

}

void handleObstacle()
{   
    turnRight();
    usleep(1500000);

    //moveForward();
    brake();
    while(1){
        printf("%d\n", echoDistance);
        usleep(1000000);
    }
    brake();
    // while(1)
    // {
    // if(obstDistance < echoDistance) 
    // {
    //     turnRight();
    // usleep(1500000);
    //     printf("distance too far %d\n", echoDistance);
    //     printf("distance from obstacle %d\n", obstDistance);
    //     brake();
    //     usleep(10000000);
    // }
    // }


    //lineRightValue == 0 && lineLeftValue == 0
    // while(1)
    // {
    //     moveForward();
    //     usleep(100000);
    //     if(echoDistance > obstDistance)
    //     {
    //         brake();
    //         printf("distance too far %d\n", echoDistance);
    //         printf("distance from obstacle %d\n", obstDistance);
    //         usleep(500000000);
    //         // turnLeft();
    //     }
    //     // } else if(echoDistance < obstDistance)
    //     // {
    //     //     // turnRight();
    //     //     // printf("distance too close %d\n", echoDistance);
    //     //     // printf("distance from obstacle %d\n", obstDistance);
    //     // } else 
    //     // {
    //     //     // moveForward();
    //     //     // printf("moving forward %d\n", echoDistance);
    //     //     // printf("distance from obstacle %d\n", obstDistance);
    //     // }
        
    //     usleep(100000);
    // }
    



    // gpioTerminate();
    // DEV_ModuleExit();
    usleep(500000000);


    // printf("First tank right\n");
    // turnRight();
    // usleep(1200000);
    // moveForward();
    // usleep(900000);
    // turnLeft();         //second turn
    // usleep(1400000);
    // moveForward();
    // usleep(1900000);
    // turnLeft();          //third turn
    // usleep(1200000);
    // moveForward();
    // usleep(1400000);
    // turnRight();
    // usleep(1000000);


    // while(lineLeftValue != 0) {
    //     sharpTurnRight();
    // }
    
    // while(leftFrontIrValue != 0) {}
    // moveForward();
    // while(leftFrontIrValue == 0) {} 
    // while(leftMidIrValue != 0) {}   //while not detecting, turn right
    // printf("Moving off course\n");
    // moveForward();
    // usleep(100000);
    // while(rightFrontIrValue == 0) {}  //while detecting, go forward
    // printf("First tank left\n");
    // turnLeft();
    // while(leftFrontIrValue != 0) {} //while not detecting, turn left 
}

int main(void)
{   
    if(initialize()) {
        printf("Initialization error\n");
        return 1;
    }
    
    PCA9685_SetPWMFreq(100, LEFT_SIDE);
    PCA9685_SetPWMFreq(100, RIGHT_SIDE);
    
    moveForward();         
    go();

    signal(SIGINT, endProgram);     //set signal handler for ctrl+c
    while(keepLooping)
    {
        // checks value obtained from ir sensor
        // and notify if an obstacle is detected 
        if(frontIrValue == 0)
        {   
            printf("Obstacle detected, ");
            handleObstacle();

        }
        go();

        // checks value obtained from line sensors
        // and notify if sensor detects a line
        while(lineRightValue != 0)
        {
            printf("turning right.\n");
            turnRight();
            printf("turned right.\n");
            while(lineRightValue != 0 && lineLeftValue != 0) {
                printf("both sensors on, in first while loop.\n");
                printf("sharp right turn.\n");
                sharpTurnRight();
                usleep(200000);
            }
            go();
            usleep(100000);
        }
        
        while(lineLeftValue != 0)
        {
            printf("turning left.\n");
            turnLeft();
            printf("turned left.\n");
            while(lineRightValue != 0 && lineLeftValue != 0) {
                printf("both sensors on, in second while loop.\n");
                printf("sharp left turn.\n");
                sharpTurnLeft();
                usleep(200000);
            }
            go();
            usleep(100000);
        }
        moveForward();
        // usleep(100000);
    }

    brake();

    gpioTerminate();
    DEV_ModuleExit();
    return 0;

    // // Duty cycle decrease by 5% every 0.3s, down to 15%
    // // Motor speed subsequently decreases down to 15%
    // for(int i = 5; (MAX_RIGHT_SPEED - i) >= 15; i += 5)
    // {
    //     PCA9685_SetPwmDutyCycle(PWMA, MAX_RIGHT_SPEED - i, LEFT_SIDE);
    //     PCA9685_SetPwmDutyCycle(PWMB, MAX_RIGHT_SPEED - i, LEFT_SIDE);

    //     PCA9685_SetPwmDutyCycle(PWMA, MAX_RIGHT_SPEED - i, RIGHT_SIDE);
    //     PCA9685_SetPwmDutyCycle(PWMB, MAX_RIGHT_SPEED - i, RIGHT_SIDE);
    //     usleep(300000);
    // }

    
    
    // Restart the motor slowly, in reverse direction
    // Increase duty cycle by 5% every 0.3 second, until motor is at max speed
    // for(int i = 0; i <= MAX_RIGHT_SPEED; i += 5)
    // {
    //     PCA9685_SetPwmDutyCycle(PWMA, i);
    //     PCA9685_SetPwmDutyCycle(PWMB, i);
    //     usleep(300000);
    // }

    //close libraries and free all mallocs
}


// int main()
// {

//     gpioSetMode(LINE_OUT, PI_INPUT);
//     threadInfo lineThread;                 // create new struct to hold info for line thread
//     lineThread.pinNumber = LINE_OUT;       // this thread will run for line sensor
//     lineThread.valuePointer = &lineValue;  // this thread will modify line readings
        
//     // cleanup threads and exit program
//     if(pthread_join(frontIrValue.id, NULL) || pthread_join(lineThread.id, NULL))
//     {
//         perror("\nProblem cleaning up threads.");
//         return 1;
//     }

//     printf("\nClosed program successfully.");
//     return 0;
    
// }



