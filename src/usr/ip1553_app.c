/*******************************************************************************
  IP1553 Application Source File

  Summary:
    IP1553 Bus Controller (BC) mode application layer.

  Description:
    This file implements BC mode communication functions for MIL-STD-1553B.
*******************************************************************************/

#include "ip1553_app.h"
#include <string.h>
#include <stdio.h>

// *****************************************************************************
// Section: IP1553 Buffer Definitions
// *****************************************************************************

/* TX Buffers - 32 buffers x 32 words each */
static uint16_t __attribute__((aligned(4))) ip1553TxBuffers[IP1553_BUFFERS_NUM][IP1553_BUFFERS_SIZE];

/* RX Buffers - 32 buffers x 32 words each */
static uint16_t __attribute__((aligned(4))) ip1553RxBuffers[IP1553_BUFFERS_NUM][IP1553_BUFFERS_SIZE];

// *****************************************************************************
// Section: IP1553 Application Implementation
// *****************************************************************************

void IP1553_App_Initialize(void)
{
    /* Clear buffers */
    memset(ip1553TxBuffers, 0, sizeof(ip1553TxBuffers));
    memset(ip1553RxBuffers, 0, sizeof(ip1553RxBuffers));
    
    /* Initialize IP1553 peripheral (already done in SYS_Initialize) */
    /* IP1553_Initialize(); */
    
    /* Set buffer addresses */
    IP1553_BuffersConfigSet((uint16_t*)ip1553TxBuffers, (uint16_t*)ip1553RxBuffers);
    
    /* Reset all buffer status - mark as ready */
    IP1553_ResetTxBuffersStatus(0xFFFFFFFF);
    IP1553_ResetRxBuffersStatus(0xFFFFFFFF);
    
    printf("IP1553 BC Mode Initialized\r\n");
}

bool IP1553_App_SendToRT(uint8_t rtAddr, uint8_t subAddr, uint16_t* data, uint8_t wordCount)
{
    if (rtAddr == 0 || rtAddr > 30 || subAddr == 0 || subAddr > 30)
    {
        return false;
    }
    
    if (wordCount == 0 || wordCount > 32)
    {
        return false;
    }
    
    /* Copy data to TX buffer (using subAddr as buffer index) */
    memcpy(ip1553TxBuffers[subAddr], data, wordCount * sizeof(uint16_t));
    
    /* Mark buffer as ready to send */
    IP1553_ResetTxBuffersStatus(IP1553_BUFFER_TO_BITFIELD_SA(subAddr));
    
    /* Start BC -> RT transfer
     * txAddr = 0 (BC is transmitter)
     * rxAddr = rtAddr (RT is receiver)
     */
    IP1553_BcStartDataTransfer(
        IP1553_DATA_TX_TYPE_BC_TO_RT,
        0,                  /* txAddr: BC = 0 */
        subAddr,            /* txSubAddr */
        rtAddr,             /* rxAddr: target RT */
        subAddr,            /* rxSubAddr */
        wordCount,          /* dataWordCount */
        IP1553_BUS_A        /* Bus A */
    );
    
    return true;
}

bool IP1553_App_ReceiveFromRT(uint8_t rtAddr, uint8_t subAddr, uint16_t* data, uint8_t wordCount)
{
    if (rtAddr == 0 || rtAddr > 30 || subAddr == 0 || subAddr > 30)
    {
        return false;
    }
    
    if (wordCount == 0 || wordCount > 32)
    {
        return false;
    }
    
    /* Mark RX buffer as ready to receive */
    IP1553_ResetRxBuffersStatus(IP1553_BUFFER_TO_BITFIELD_SA(subAddr));
    
    /* Start RT -> BC transfer
     * txAddr = rtAddr (RT is transmitter)
     * rxAddr = 0 (BC is receiver)
     */
    IP1553_BcStartDataTransfer(
        IP1553_DATA_TX_TYPE_RT_TO_BC,
        rtAddr,             /* txAddr: source RT */
        subAddr,            /* txSubAddr */
        0,                  /* rxAddr: BC = 0 */
        subAddr,            /* rxSubAddr */
        wordCount,          /* dataWordCount */
        IP1553_BUS_A        /* Bus A */
    );
    
    /* Note: Caller must wait for transfer complete and then copy data */
    /* Data will be in ip1553RxBuffers[subAddr] after transfer complete */
    
    return true;
}

bool IP1553_App_SendModeCommand(uint8_t rtAddr, IP1553_MODE_CMD modeCmd)
{
    if (rtAddr == 0 || rtAddr > 31)  /* 31 = broadcast */
    {
        return false;
    }
    
    /* Send mode command to RT */
    IP1553_BcModeCommandTransfer(rtAddr, modeCmd, 0, IP1553_BUS_A);
    
    return true;
}

IP1553_INT_MASK IP1553_App_CheckStatus(void)
{
    IP1553_INT_MASK status = IP1553_IrqStatusGet();
    
    /* Check for errors */
    if (status & IP1553_INT_MASK_ERROR_MASK)
    {
        printf("IP1553 Error: 0x%08lX\r\n", (unsigned long)status);
        
        if (status & IP1553_INT_MASK_MTE) printf("  - Memory Transfer Error\r\n");
        if (status & IP1553_INT_MASK_TE)  printf("  - Transfer Error\r\n");
        if (status & IP1553_INT_MASK_TCE) printf("  - Transfer Coding Error\r\n");
        if (status & IP1553_INT_MASK_TPE) printf("  - Transfer Parity Error\r\n");
        if (status & IP1553_INT_MASK_TDE) printf("  - Transfer Data Type Error\r\n");
        if (status & IP1553_INT_MASK_TTE) printf("  - Transfer Timeout Error\r\n");
        if (status & IP1553_INT_MASK_TWE) printf("  - Transfer Word Count Error\r\n");
        if (status & IP1553_INT_MASK_BE)  printf("  - Buffer Interface Error\r\n");
        if (status & IP1553_INT_MASK_ITR) printf("  - Illegal Transfer Request\r\n");
    }
    
    /* Check for successful completion */
    if (status & IP1553_INT_MASK_ETX)
    {
        printf("IP1553: Transmission Complete\r\n");
    }
    
    if (status & IP1553_INT_MASK_ERX)
    {
        printf("IP1553: Reception Complete\r\n");
    }
    
    return status;
}

void IP1553_App_Test(void)
{
    uint16_t testData[8] = {0x1234, 0x5678, 0xABCD, 0xEF01, 
                            0x2345, 0x6789, 0xBCDE, 0xF012};
    
    printf("\r\n=== IP1553 BC Mode Test ===\r\n");
    
    /* Initialize */
    IP1553_App_Initialize();
    
    /* Test 1: Send data to RT1, SubAddr 1 */
    printf("\r\nTest 1: Sending 8 words to RT1, SA1...\r\n");
    if (IP1553_App_SendToRT(1, 1, testData, 8))
    {
        printf("  Command sent.\r\n");
        
        /* Wait a bit and check status */
        delay_ms(10);
        IP1553_App_CheckStatus();
    }
    else
    {
        printf("  Failed to send command.\r\n");
    }
    
    /* Test 2: Mode command - Transmit Status Word */
    printf("\r\nTest 2: Mode Command - Transmit Status Word to RT1...\r\n");
    if (IP1553_App_SendModeCommand(1, IP1553_MODE_CMD_TRANSMIT_STATUS_WORD))
    {
        printf("  Command sent.\r\n");
        delay_ms(10);
        
        /* Get status words */
        uint16_t statusWord1 = IP1553_GetFirstStatusWord();
        printf("  RT Status Word: 0x%04X\r\n", statusWord1);
    }
    
    printf("\r\n=== Test Complete ===\r\n");
}

/* Get received data after transfer complete */
void IP1553_App_GetReceivedData(uint8_t subAddr, uint16_t* data, uint8_t wordCount)
{
    if (subAddr < IP1553_BUFFERS_NUM && wordCount <= IP1553_BUFFERS_SIZE)
    {
        memcpy(data, ip1553RxBuffers[subAddr], wordCount * sizeof(uint16_t));
    }
}
