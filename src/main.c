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
#include <stdio.h>
#include <inttypes.h>
#include "definitions.h"                // SYS function prototypes
#include "peripheral/xdmac/plib_xdmac_common.h"
#include "usr/default_function.h"       // Default functions
#include "usr/ip1553_rt.h"              // IP1553 RT Mode Application
#include "usr/circular_dma.h"           // Circular DMA 함수
#include "usr/FLEXCOM_dma_types.h"     // FLEXCOM DMA 객체 타입

/* Callback for logging DMA0 Events *//*
void DmaCh0Callback(XDMAC_TRANSFER_EVENT event, uintptr_t context)
{
    FLEXCOM_DMA_OBJECT* obj = (FLEXCOM_DMA_OBJECT*)context;

    if (obj == NULL)
    {
        return;
    }

    if (event == XDMAC_TRANSFER_COMPLETE)
    {
        obj->rxByteCount += obj->rxDescriptor.mbr_ubc.blockDataLength;
        obj->rxPacketCount++;
    }
    else if (event == XDMAC_TRANSFER_ERROR)
    {
        obj->overrunCount++;
    }
}
*/

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    /* Recover from stale/stuck state before first science TX scheduling. */
    XDMAC_ChannelDisable(FLEXCOM_TX_XDMAC_CHANNEL);

    /* Register button interrupt callbacks */
    PIO_PinInterruptCallbackRegister(PIO_PIN_PC29, PlayCallback, 0);
    PIO_PinInterruptCallbackRegister(PIO_PIN_PC30, Button_Callback, 0);
    PIO_PinInterruptCallbackRegister(PIO_PIN_PC31, Button_Callback, 0);
    // XDMAC_ChannelCallbackRegister(FLEXCOM0_RX_XDMAC_CHANNEL, DmaCh0Callback, (uintptr_t)&FLEXCOM0DmaObj);
    // XDMAC_ChannelCallbackRegister(FLEXCOM2_RX_XDMAC_CHANNEL, DmaCh0Callback, (uintptr_t)&FLEXCOM2DmaObj);
    // XDMAC_ChannelCallbackRegister(FLEXCOM3_RX_XDMAC_CHANNEL, DmaCh0Callback, (uintptr_t)&FLEXCOM3DmaObj);
    // XDMAC_ChannelCallbackRegister(FLEXCOM4_RX_XDMAC_CHANNEL, DmaCh0Callback, (uintptr_t)&FLEXCOM4DmaObj);
    
    /* Enable button interrupts */
    PIO_PinInterruptEnable(PIO_PIN_PC29);
    PIO_PinInterruptEnable(PIO_PIN_PC30);
    PIO_PinInterruptEnable(PIO_PIN_PC31);
    
    /* Initialize Circular DMA for UART RX */
    CircularDMA_RX_Init(&FLEXCOM0DmaObj);
    CircularDMA_RX_Init(&FLEXCOM2DmaObj);
    CircularDMA_RX_Init(&FLEXCOM3DmaObj);
    CircularDMA_RX_Init(&FLEXCOM4DmaObj);
	


    IP1553_RT_Test();
    
    /* Wait for button press to select mode */
    while (SYSTEM_PAUSED){}
    Print_system_info();

    /* Main loop - runs after mode is selected */
    while (true)
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );

        if (SYSTEM_PAUSED)
        {
          continue;
        }
		
    	// printf("RX Packets in queue: %" PRIu32 "\r\n", FLEXCOM0DmaObj.rxPacketCount);

        PacketQueue_Push(&FLEXCOM0DmaObj);
        PacketQueue_Push(&FLEXCOM2DmaObj);
        // PacketQueue_Push(&FLEXCOM3DmaObj);
        // PacketQueue_Push(&FLEXCOM4DmaObj);
        DMA_TX_init();
      }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/