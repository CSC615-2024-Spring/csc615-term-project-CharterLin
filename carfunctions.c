/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Dat Vo, Charter Lin, Kotaro Iwanaga, Jimmie Wu
* Student ID:: 922171975, 922577939, 922782507, 921549444
* Github-Name:: CharterLin
* Project:: Term Project
*
* File:: carfunctions.c
*
* Description:: A program that uses line sensors to follow a line
* to navigate an obstacle course. One IR sensor is used to detect
* obstacles to the front of the cart. An echo sensor to the left side
* is used to navigate around the obstacle. Line sensors use threads
* to update values that the main program loop uses to keep the cart
* following the line. Echo sensor readings are obtained as needed
* when the cart navigates the obstacle.
**************************************************************/

#include "carfunctions.h"

volatile int frontIrValue = 1;      // input value from ir sensor
volatile int lineRightValue = 0;    // input value from right line sensor
volatile int lineLeftValue = 0;     // input value from left line sensor
volatile int keepLooping = 1;       // tells program when to finish and exit

typedef struct threadInfo
{
    pthread_t id;                   // thread id returned from thread creation
    unsigned int pinNumber;         // pin that the thread will read from
    volatile int* valuePointer;     // pointer to the integer value that the thread will modify
} threadInfo;

threadInfo* frontIrThread;          // thread info container for the front IR sensor
threadInfo* lineRightThread;        // thread info container for the right side line sensor
threadInfo* lineLeftThread;         // thread info container for the right side line sensor

// thread routine for line sensors: reads pin value from given pin number and transfers that value to
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

/* 
Function that measures distance of the portside of the cart from the obstacle.
By recording the time in which the echo pin is pulled up and then pulled down,
the program can find the duration of the ultrasonic's travel time. The 
distance traveled can then be calculated using the measured
duration and the sound velocity constant(in cm/uS). 
*/
int readEcho()
{
    struct timeval start, end;

    int echoDistance = 0;

        gpioWrite(TRIGGER, 1);
        usleep(10);
        gpioWrite(TRIGGER, 0);

        while(gpioRead(ECHO) == 0) {}
        gettimeofday(&start, NULL);

        while(gpioRead(ECHO) == 1) {}
        gettimeofday(&end, NULL); 

        suseconds_t duration = end.tv_usec - start.tv_usec;

        echoDistance = ((int)duration * VELOCITY) / 2; 
        printf("distance %d\n", echoDistance);

        return(echoDistance);    
}

// Create threads for front ir and left, right line sensors
int createThreads()
{
    if(pthread_create(&frontIrThread->id, NULL, &readSensor, (void*) frontIrThread) != 0
    || pthread_create(&lineRightThread->id, NULL, &readSensor, (void*) lineRightThread) != 0
    || pthread_create(&lineLeftThread->id, NULL, &readSensor, (void*) lineLeftThread) != 0)
    {
        perror("Thread creation error");
        return 1;
    }

    return 0;
}

// Join front ir and left, right line sensors threads
void joinThreads()
{
    if(pthread_join(frontIrThread->id, NULL)
    || pthread_join(lineRightThread->id, NULL)
    || pthread_join(lineLeftThread->id, NULL))
    {
        perror("\nProblem cleaning up threads.");
    }
}

// Initialize PCA and gpio libraries. Set pin modes and create thread info structs
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

    gpioSetMode(BUTTON, PI_INPUT);
    gpioSetPullUpDown(BUTTON, PI_PUD_UP);

    gpioSetMode(IR_OUT, PI_INPUT);
    frontIrThread = malloc(sizeof(threadInfo));
    frontIrThread->pinNumber = IR_OUT;            // this thread will run for ir sensor
    frontIrThread->valuePointer = &frontIrValue;  // this thread will modify ir readings

    gpioSetMode(LINE_RIGHT_OUT, PI_INPUT);
    lineRightThread = malloc(sizeof(threadInfo));
    lineRightThread->pinNumber = LINE_RIGHT_OUT;      // this thread will run for right line sensor 
    lineRightThread->valuePointer = &lineRightValue;  // this thread will modify right line readings

    gpioSetMode(LINE_LEFT_OUT, PI_INPUT);
    lineLeftThread = malloc(sizeof(threadInfo));
    lineLeftThread->pinNumber = LINE_LEFT_OUT;       // this thread will run for left line sensor 
    lineLeftThread->valuePointer = &lineLeftValue;  // this thread will modify left line readings

    gpioSetMode(TRIGGER, PI_OUTPUT);
    gpioSetMode(ECHO, PI_INPUT);

    return createThreads();
}

// Set motors directions to move forward
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

// Set motors directions to move backwards
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

// Set motors directions to tank turn right
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

// Set motors directions to turn right with left motors off
void sharpTurnRight()
{
    PCA9685_SetLevel(AIN1, 0, LEFT_SIDE);          
    PCA9685_SetLevel(AIN2, 1, LEFT_SIDE);          
    PCA9685_SetLevel(BIN1, 0, LEFT_SIDE);         
    PCA9685_SetLevel(BIN2, 1, LEFT_SIDE);          

    PCA9685_SetPwmDutyCycle(PWMA, 0, RIGHT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, 0, RIGHT_SIDE);
}

// Set motors directions to tank turn left
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

// Set motors directions to turn left with right motors off
void sharpTurnLeft()
{
    PCA9685_SetPwmDutyCycle(PWMA, 0, LEFT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, 0, LEFT_SIDE);          

    PCA9685_SetLevel(AIN1, 1, RIGHT_SIDE);          
    PCA9685_SetLevel(AIN2, 0, RIGHT_SIDE);          
    PCA9685_SetLevel(BIN1, 1, RIGHT_SIDE);         
    PCA9685_SetLevel(BIN2, 0, RIGHT_SIDE);
}

// Turn all motors on
void go()
{
    PCA9685_SetPwmDutyCycle(PWMA, MAX_LEFT_SPEED, LEFT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, MAX_LEFT_SPEED, LEFT_SIDE);

    PCA9685_SetPwmDutyCycle(PWMA, MAX_RIGHT_SPEED, RIGHT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, MAX_RIGHT_SPEED, RIGHT_SIDE);
}

// Turn all motors off
void brake()
{
    PCA9685_SetPwmDutyCycle(PWMA, 0, LEFT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, 0, LEFT_SIDE);

    PCA9685_SetPwmDutyCycle(PWMA, 0, RIGHT_SIDE);
    PCA9685_SetPwmDutyCycle(PWMB, 0, RIGHT_SIDE);
}

//crtl + c handler. Cleanup routine: turn off motors, close libraries, end threads, and exit program 
static void endProgram()
{      
    printf("shutting down\n");
    brake();
    gpioTerminate();
    DEV_ModuleExit();
    keepLooping = 0;
    joinThreads();
    exit(0);
}

/* Once main loop detects obstacle, hands control to this function
   to navigate the obstacle and merge back onto the line.
*/
void handleObstacle()
{   
    // initial 90 turn
    turnRight();
    usleep(1400000);
    
    brake();
    usleep(500000);
    go();                   //move off course

    while(readEcho() < 90)  //until echo no longer detects
    {
        moveForward();
        usleep(100000);
    }
    printf("Cleared Obstacle 1!!!!!!!!!!!!!! distance at %d\n", readEcho());

    // 2nd 90 turn
    turnLeft();
    usleep(1300000);


    brake();
    usleep(500000);
    go();

    // go a little bit, so echo detects sensor
    moveForward();
    usleep(1100000);

    while(readEcho() < 80)  // forward until pass obstacle
    {
        usleep(100000);
    }
    printf("Cleared Obstacle 2!!!!!!!!!!!!!! distance at %d\n", readEcho());

    // final 90 turn
    turnLeft();
    usleep(1200000);

    brake();
    usleep(500000);

    // forward until touches line
    go();
    moveForward();
    while(lineLeftValue == 0 && lineRightValue == 0) {
    }

    //merge onto line
    sharpTurnRight();
    usleep(1500000);

    // add go() here to do an actual turnRight()
    go();
    turnRight();        
    usleep(500000);
}

int main(void)
{   
    if(initialize()) {
        printf("Initialization error\n");
        return 1;
    }

    while(gpioRead(BUTTON) == 1) {}     // Polling for button press
    
    PCA9685_SetPWMFreq(100, LEFT_SIDE);
    PCA9685_SetPWMFreq(100, RIGHT_SIDE);
    
    moveForward();         
    go();

    signal(SIGINT, endProgram);     //set signal handler for ctrl+c
    while(1)
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

            // Both sensors are on while doing a turn, indicating a sharp angle
            // So, initiate a sharp turn until at least one sensor is off
            while(lineRightValue != 0 && lineLeftValue != 0) {
                printf("both sensors on, in first while loop.\n");
                printf("sharp right turn.\n");
                sharpTurnRight();
                usleep(400000);
            }
            go();
            usleep(100000);
        }
        
        while(lineLeftValue != 0)
        {
            printf("turning left.\n");
            turnLeft();
            printf("turned left.\n");

            // Both sensors are on while doing a turn, indicating a sharp angle
            // So, initiate a sharp turn until at least one sensor is off
            while(lineRightValue != 0 && lineLeftValue != 0) {
                printf("both sensors on, in second while loop.\n");
                printf("sharp left turn.\n");
                sharpTurnLeft();
                usleep(400000);
            }
            go();
            usleep(100000);
        }
        moveForward();
    }

    brake();

    gpioTerminate();
    DEV_ModuleExit();
    return 0;

}