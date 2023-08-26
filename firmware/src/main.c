/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
  Project pic32mx-comp - Using comparator to measure input signal frequency.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes


#define PROG_VERSION 101 // 123 = 1.23

volatile bool gTimerTicked=false;
volatile uint32_t gTimer1s = 0; // Timer incremented every second

// SYSTICK Timer interrupt, called every 1s (1 Hz rate)
void CORETIMER_EventHandler(uint32_t status, uintptr_t context)
{
    gTimerTicked=true;
    gTimer1s++;
    /* Toggle LED on PIN2 RA0 at 1 Hz rate => RA0 frequency = 1/2 Hz */
    GPIO_RA0_Toggle();
}

#define PROG_WAIT_AND_ACK_1S_TIMER \
    do{ \
    while(!gTimerTicked); \
    gTimerTicked=false; \
}while(0)

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    uint32_t oldCount=0,count;
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    printf("%s:%d starting app v%d.%02d\r\n",
            __FILE__,__LINE__,PROG_VERSION/100,PROG_VERSION%100);
    CORETIMER_CallbackSet(CORETIMER_EventHandler,(uintptr_t)NULL);
    CORETIMER_Start();
    // poor attempt to measure CPU frequency
    PROG_WAIT_AND_ACK_1S_TIMER;
    oldCount=_CP0_GET_COUNT();
    PROG_WAIT_AND_ACK_1S_TIMER;
    count = _CP0_GET_COUNT();
    // we must multiply by 2 because Core Timer ticks every *second* Cycle
    printf(" CPU Frequency=%u [Hz]\r\n",2*(count-oldCount));
    while ( true )
    {
        PROG_WAIT_AND_ACK_1S_TIMER;
        // show uptime
        // TODO: Show also comparator Interrupt count (signal frequency)
        printf(" Uptime=%u [s]\r\n", gTimer1s);
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/