/*******************************************************************************
  UART DMA Objects Source File

  File Name:
    FLEXCOM_dma_objects.c

  Summary:
    Global FLEXCOM DMA object definitions (Polling-based)
 *******************************************************************************/

#include "FLEXCOM_dma_types.h"

// *****************************************************************************
// Section: Global Variables - RX Objects
// *****************************************************************************

FLEXCOM_DMA_OBJECT FLEXCOM0DmaObj = {
    .headerIdx = 0x00,
    .dataIdx = 0,
    .SDQueueTail = 0,
    .rxChannel = FLEXCOM0_RX_XDMAC_CHANNEL,
    .rxReg = (const void*)&(FLEXCOM0_REGS->FLEX_US_RHR),
    .rxByteCount = 0,
    .rxPacketCount = 0,
    .overrunCount = 0,
    .parseErrorCount = 0
};

FLEXCOM_DMA_OBJECT FLEXCOM2DmaObj = {
    .headerIdx = 0x02,
    .dataIdx = 0,
    .SDQueueTail = 0,
    .rxChannel = FLEXCOM2_RX_XDMAC_CHANNEL,
    .rxReg = (const void*)&(FLEXCOM2_REGS->FLEX_US_RHR),
    .rxByteCount = 0,
    .rxPacketCount = 0,
    .overrunCount = 0,
    .parseErrorCount = 0
};

FLEXCOM_DMA_OBJECT FLEXCOM3DmaObj = {
    .headerIdx = 0x03,
    .dataIdx = 0,
    .SDQueueTail = 0,
    .rxChannel = FLEXCOM3_RX_XDMAC_CHANNEL,
    .rxReg = (const void*)&(FLEXCOM3_REGS->FLEX_US_RHR),
    .rxByteCount = 0,
    .rxPacketCount = 0,
    .overrunCount = 0,
    .parseErrorCount = 0
};

FLEXCOM_DMA_OBJECT FLEXCOM4DmaObj = {
    .headerIdx = 0x04,
    .dataIdx = 0,
    .SDQueueTail = 0,
    .rxChannel = FLEXCOM4_RX_XDMAC_CHANNEL,
    .rxReg = (const void*)&(FLEXCOM4_REGS->FLEX_US_RHR),
    .rxByteCount = 0,
    .rxPacketCount = 0,
    .overrunCount = 0,
    .parseErrorCount = 0
};

PACKET_QUEUE FLEXCOM1PacketQueue = { .PckQHead = 0, .PckQTail = 0 };