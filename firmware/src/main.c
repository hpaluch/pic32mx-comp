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


#define PROG_VERSION 100 // 123 = 1.23

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
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    printf("%s:%d starting app v%d.%02d\r\n",
            __FILE__,__LINE__,PROG_VERSION/100,PROG_VERSION%100);
    CORETIMER_CallbackSet(CORETIMER_EventHandler,(uintptr_t)NULL);
    CORETIMER_Start();
    while ( true )
    {
        while(!gTimerTicked); // wait till Core Timer ticked (1s)
        gTimerTicked=false;
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