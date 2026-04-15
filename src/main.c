/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

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
#include "usr/default_function.h"       // Default functions
#include "usr/ip1553_app.h"             // IP1553 Application

// Global mode variable (defined here, declared extern in default_function.h)
volatile uint8_t MODE = 0;              // 0: waiting, 1: Normal, 2: 1553 Test, 3: Mode2

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    /* Register button interrupt callbacks */
    PIO_PinInterruptCallbackRegister(PIO_PIN_PC29, Button_Callback, 0);
    PIO_PinInterruptCallbackRegister(PIO_PIN_PC30, Button_Callback, 0);
    PIO_PinInterruptCallbackRegister(PIO_PIN_PC31, Button_Callback, 0);
    
    /* Enable button interrupts */
    PIO_PinInterruptEnable(PIO_PIN_PC29);
    PIO_PinInterruptEnable(PIO_PIN_PC30);
    PIO_PinInterruptEnable(PIO_PIN_PC31);
    
    /* Wait for button press to select mode */
    
    while (MODE == 0){}
    Print_system_info();
    
    /* Initialize IP1553 if mode 2 selected */
    if (MODE == 2)
    {
        IP1553_App_Test();
    }
    
    /* Main loop - runs after mode is selected */
    while (true)
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        /* Mode-specific tasks */
        switch (MODE)
        {
            case 1:  /* Normal Mode */
                break;
                
            case 2:  /* 1553 Test Mode */
                /* IP1553 status polling can be added here */
                break;
                
            case 3:  /* Mode 2 */
                break;
                
            default:
                break;
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/