/*******************************************************************************
  IP1553 RT (Remote Terminal) Application Source File

  Summary:
    IP1553 Remote Terminal mode application layer.

  Description:
    This file implements RT mode communication functions for MIL-STD-1553B.
    RT responds to Bus Controller commands.
*******************************************************************************/

#include "ip1553_rt.h"
#include "component/ip1553.h"
#include "default_function.h"
#include <string.h>
#include <stdio.h>

// *****************************************************************************
// Section: IP1553 RT Configuration
// *****************************************************************************

/* Our RT Address (1-30, set in MCC or here) */
#define IP1553_RT_ADDRESS               IP1553_RT_ADDRESS_BROADCAST_MODE

// *****************************************************************************
// Section: IP1553 Buffer Definitions
// *****************************************************************************

/* TX Buffers - 32 buffers x 32 words each (data RT sends to BC) */
static uint16_t __attribute__((aligned(4))) ip1553TxBuffers[IP1553_BUFFERS_NUM][IP1553_BUFFERS_SIZE];

/* RX Buffers - 32 buffers x 32 words each (data RT receives from BC) */
static uint16_t __attribute__((aligned(4))) ip1553RxBuffers[IP1553_BUFFERS_NUM][IP1553_BUFFERS_SIZE];

/* RT Status */
static volatile bool rtInitialized = false;
static volatile uint32_t rtRxCount = 0;
static volatile uint32_t rtTxCount = 0;

static void IP1553_RT_Callback(uintptr_t contextHandle)
{
    (void)contextHandle;
    (void)IP1553_RT_ProcessEvents();
}

// *****************************************************************************
// Section: IP1553 RT Mode Implementation
// *****************************************************************************

void IP1553_RT_Initialize(uint8_t rtAddress)
{
    /* Clear buffers */
    memset(ip1553TxBuffers, 0, sizeof(ip1553TxBuffers));
    memset(ip1553RxBuffers, 0, sizeof(ip1553RxBuffers));

    /* Configure RT address before enabling the interrupt path */
    IP1553_REGS->IP1553_CR = IP1553_CR_TA(rtAddress);
    
    /* Step 1: Set buffer addresses */
    IP1553_BuffersConfigSet((uint16_t*)ip1553TxBuffers, (uint16_t*)ip1553RxBuffers);
    
    /* Step 2: Reset all buffer status - mark as ready */
    IP1553_ResetTxBuffersStatus(0xFFFFFFFF);
    IP1553_ResetRxBuffersStatus(0xFFFFFFFF);

    /* Route the generated peripheral ISR into the RT event processor */
    IP1553_CallbackRegister(IP1553_RT_Callback, (uintptr_t)0);
    
    /* Step 3: Enable individual interrupts in IER */
    IP1553_REGS->IP1553_IER = IP1553_IER_EMT(1) |  /* End Memory Transfer */
                              IP1553_IER_ERX(1) |  /* End Reception */
                              IP1553_IER_ETX(1) |  /* End Transmission */
                              IP1553_IER_TE(1)  |  /* Transfer Error */
                              IP1553_IER_MTE(1);   /* Memory Transfer Error */
    
    rtInitialized = true;
    rtRxCount = 0;
    rtTxCount = 0;
    
    printf("IP1553 RT Mode Initialized (Address: %d)\r\n", rtAddress);
}

void IP1553_RT_SetTxData(uint8_t subAddr, uint16_t* data, uint8_t wordCount)
{
    if (subAddr >= IP1553_BUFFERS_NUM || wordCount > IP1553_BUFFERS_SIZE)
    {
        return;
    }
    
    /* Copy data to TX buffer for this sub-address */
    memcpy(ip1553TxBuffers[subAddr], data, wordCount * sizeof(uint16_t));
    
    /* Mark buffer as ready to send */
    IP1553_ResetTxBuffersStatus(IP1553_BUFFER_TO_BITFIELD_SA(subAddr));
}

void IP1553_RT_GetRxData(uint8_t subAddr, uint16_t* data, uint8_t wordCount)
{
    if (subAddr >= IP1553_BUFFERS_NUM || wordCount > IP1553_BUFFERS_SIZE)
    {
        return;
    }
    
    /* Copy data from RX buffer */
    memcpy(data, ip1553RxBuffers[subAddr], wordCount * sizeof(uint16_t));
}

void IP1553_RT_ResetRxBuffer(uint8_t subAddr)
{
    /* Mark RX buffer as ready to receive new data */
    IP1553_ResetRxBuffersStatus(IP1553_BUFFER_TO_BITFIELD_SA(subAddr));
}

IP1553_INT_MASK IP1553_RT_ProcessEvents(void)
{
    IP1553_INT_MASK status = IP1553_IrqStatusGet();
    
    if (status == 0)
    {
        return 0;
    }
    
    /* Check for errors */
    if (status & IP1553_INT_MASK_ERROR_MASK)
    {
        printf("IP1553 RT Error: 0x%08lX\r\n", (unsigned long)status);
        
        if (status & IP1553_INT_MASK_MTE) printf("  - Memory Transfer Error\r\n");
        if (status & IP1553_INT_MASK_TE)  printf("  - Transfer Error\r\n");
        if (status & IP1553_INT_MASK_TCE) printf("  - Transfer Coding Error\r\n");
        if (status & IP1553_INT_MASK_TPE) printf("  - Transfer Parity Error\r\n");
        if (status & IP1553_INT_MASK_TTE) printf("  - Transfer Timeout Error\r\n");
    }
    
    /* Reception complete (BC -> RT transfer done) */
    if (status & IP1553_INT_MASK_ERX)
    {
        rtRxCount++;
        printf("IP1553 RT: Data Received (count: %lu)\r\n", (unsigned long)rtRxCount);
        
        /* Check which RX buffers have new data */
        uint32_t rxStatus = IP1553_GetRxBuffersStatus();
        printf("  RX Buffer Status: 0x%08lX\r\n", (unsigned long)rxStatus);
    }
    
    /* Transmission complete (RT -> BC transfer done) */
    if (status & IP1553_INT_MASK_ETX)
    {
        rtTxCount++;
        printf("IP1553 RT: Data Sent (count: %lu)\r\n", (unsigned long)rtTxCount);
    }
    
    /* Mode command received */
    if (status & IP1553_INT_MASK_STR)  /* Self Test Request */
    {
        printf("IP1553 RT: Self Test Request received\r\n");
    }
    
    if (status & IP1553_INT_MASK_TSR)  /* Transmitter Shutdown Request */
    {
        printf("IP1553 RT: Transmitter Shutdown Request received\r\n");
    }
    
    if (status & IP1553_INT_MASK_RRT)  /* Reset RT Request */
    {
        printf("IP1553 RT: Reset RT Request received\r\n");
    }
    
    return status;
}

uint32_t IP1553_RT_GetRxCount(void)
{
    return rtRxCount;
}

uint32_t IP1553_RT_GetTxCount(void)
{
    return rtTxCount;
}

void IP1553_RT_Test(void)
{
    printf("\r\n=== IP1553 RT Mode Test ===\r\n");
    
    /* Initialize RT with address */
    IP1553_RT_Initialize(IP1553_RT_ADDRESS);
    
    /* Prepare test data in TX buffers (for when BC requests data) */
    uint16_t testData[8] = {0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD,
                            0x1111, 0x2222, 0x3333, 0x4444};
    
    /* Set data for sub-address 1 */
    IP1553_RT_SetTxData(1, testData, 8);
    printf("TX buffer SA1 loaded with test data\r\n");
    
    /* Set data for sub-address 2 */
    testData[0] = 0x5555;
    testData[1] = 0x6666;
    IP1553_RT_SetTxData(2, testData, 8);
    printf("TX buffer SA2 loaded with test data\r\n");
    
    /* Reset RX buffers to receive data */
    IP1553_RT_ResetRxBuffer(1);
    IP1553_RT_ResetRxBuffer(2);
    printf("RX buffers SA1, SA2 ready to receive\r\n");
    
    printf("\r\nRT is ready. Waiting for BC commands...\r\n");
    printf("Press any button to check status.\r\n");
    printf("=================================\r\n");
}

/* Main loop polling function - call this in while(true) loop */
void IP1553_RT_Poll(void)
{
    if (!rtInitialized)
    {
        return;
    }
    
    /* Process any pending events */
    IP1553_RT_ProcessEvents();
}

/* Print current RT status */
void IP1553_RT_PrintStatus(void)
{
    printf("\r\n--- IP1553 RT Status ---\r\n");
    printf("  RX Count: %lu\r\n", (unsigned long)rtRxCount);
    printf("  TX Count: %lu\r\n", (unsigned long)rtTxCount);
    printf("  TX Buffer Status: 0x%08lX\r\n", (unsigned long)IP1553_GetTxBuffersStatus());
    printf("  RX Buffer Status: 0x%08lX\r\n", (unsigned long)IP1553_GetRxBuffersStatus());
    printf("  ISR: 0x%08lX\r\n", (unsigned long)IP1553_IrqStatusGet());
    printf("------------------------\r\n");
}
