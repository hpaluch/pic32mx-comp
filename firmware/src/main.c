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


#define PROG_VERSION 103 // 123 = 1.23

volatile bool gTimerTicked=false;
volatile uint32_t gTimer1s = 0; // Timer incremented every second
volatile uint32_t gCmp1Count = 0; // counting Comparator 1 interrupts

// SYSTICK Timer interrupt, called every 1s (1 Hz rate)
void CORETIMER_EventHandler(uint32_t status, uintptr_t context)
{
    gTimerTicked=true;
    gTimer1s++;
    /* Toggle LED on PIN2 RA0 at 1 Hz rate => RA0 frequency = 1/2 Hz */
    GPIO_RA0_Toggle();
}

// Comparator1 Interrupt handler
void CMP1_EventHandler(uintptr_t context)
{
    // Toggle output on PIN3 RA1 to show interrupt latency
    GPIO_RA1_Toggle();
    gCmp1Count++;
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
    uint32_t oldCmp1Count=0,cmp1Count;
    uint32_t cmpDelta;
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    printf("%s:%d starting app v%d.%02d\r\n",
            __FILE__,__LINE__,PROG_VERSION/100,PROG_VERSION%100);
    CORETIMER_CallbackSet(CORETIMER_EventHandler,(uintptr_t)NULL);
    CORETIMER_Start();
    CMP_1_CallbackRegister(CMP1_EventHandler,(uintptr_t)NULL);
    // Yes, must enable Voltage reference manually
    CVR_Start (); // should output 1.65V on PIN25 CVREFOUT
    // And also has to enable Comparator 1
    CMP_1_CompareEnable();
    
    // poor attempt to measure CPU frequency
    PROG_WAIT_AND_ACK_1S_TIMER;
    oldCount=_CP0_GET_COUNT();
    PROG_WAIT_AND_ACK_1S_TIMER;
    oldCmp1Count=gCmp1Count;
    count = _CP0_GET_COUNT();
    // we must multiply by 2 because Core Timer ticks every *second* Cycle
    printf(" CPU Frequency=%u [Hz]\r\n",2*(count-oldCount));
    while ( true )
    {
        PROG_WAIT_AND_ACK_1S_TIMER;
        cmp1Count = gCmp1Count;
        cmpDelta = cmp1Count-oldCmp1Count;
        oldCmp1Count=cmp1Count;
        // show uptime + delta + freq
        printf(" Uptime=%u [s] delta CMP1=%u [Hz]\r\n",
                gTimer1s,cmpDelta);
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/