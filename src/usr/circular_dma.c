/*******************************************************************************
  Circular DMA Source File (Polling-based)

  File Name:
    circular_dma.c

  Summary:
    Circular DMA implementation - No callbacks, polling-based state machine
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "circular_dma.h"

static uint8_t g_txHeader[HEADER_SIZE];

static FLEXCOM_DMA_OBJECT* GetDmaObjByHeader(uint8_t chId)
{
    if (FLEXCOM0DmaObj.headerIdx == chId) return &FLEXCOM0DmaObj;
    if (FLEXCOM2DmaObj.headerIdx == chId) return &FLEXCOM2DmaObj;
    if (FLEXCOM3DmaObj.headerIdx == chId) return &FLEXCOM3DmaObj;
    if (FLEXCOM4DmaObj.headerIdx == chId) return &FLEXCOM4DmaObj;
    return NULL;
}

// *****************************************************************************
// Section: Debug Bypass (Non-Release Mode)
// *****************************************************************************
#ifndef __RELEASE__
#endif

// *****************************************************************************
// Section: DMA Functions
// *****************************************************************************

void CircularDMA_RX_Init(FLEXCOM_DMA_OBJECT* obj)
{
    obj->SDQueueTail = 0;
    memset(obj->SDQueue, 0, sizeof(obj->SDQueue));
    
    // 캐시 무효화 (버퍼 영역)
    SCB_InvalidateDCache_by_Addr((uint32_t*)obj->SDQueue, BUFFER_SIZE);
    
    // ========================================
    // Self-Linked Descriptor 설정 (Circular DMA)
    // ========================================
    // View 0: NDA, UBC, DA만 포함 (SA는 채널 설정에서 Peripheral로 고정)
    
    // 다음 descriptor = 자기 자신 → 무한 순환
    obj->rxDescriptor.mbr_nda = (uint32_t)&obj->rxDescriptor;
    
    // 목적지 = rxBuffer 시작 주소
    obj->rxDescriptor.mbr_da = (uint32_t)obj->SDQueue;
    
    // Micro Block Control 설정
    obj->rxDescriptor.mbr_ubc.blockDataLength = BUFFER_SIZE;
    obj->rxDescriptor.mbr_ubc.nextDescriptorControl.fetchEnable = 1;       // 다음 descriptor fetch
    obj->rxDescriptor.mbr_ubc.nextDescriptorControl.sourceUpdate = 0;      // SA 업데이트 안함 (Peripheral 고정)
    obj->rxDescriptor.mbr_ubc.nextDescriptorControl.destinationUpdate = 1; // DA 업데이트 (버퍼 시작으로)
    obj->rxDescriptor.mbr_ubc.nextDescriptorControl.view = 0;              // View 0 사용
    
    // Descriptor 캐시 클린 (Memory에 확실히 쓰기)
    SCB_CleanDCache_by_Addr((uint32_t*)&obj->rxDescriptor, sizeof(XDMAC_DESCRIPTOR_VIEW_0));
    
    // ========================================
    // Source Address 설정 (View 0에는 SA가 없으므로 수동 설정)
    // ========================================
    XDMAC_REGS->XDMAC_CHID[obj->rxChannel].XDMAC_CSA = (uint32_t)obj->rxReg;
    
    // ========================================
    // Linked List DMA 시작
    // ========================================
    // 첫 번째 descriptor의 제어 정보 (첫 fetch 시 적용)
    XDMAC_DESCRIPTOR_CONTROL firstControl = obj->rxDescriptor.mbr_ubc.nextDescriptorControl;
    
    bool result = XDMAC_ChannelLinkedListTransfer(
        obj->rxChannel,
        (uint32_t)&obj->rxDescriptor,
        &firstControl
    );
    
    if (result)
    {
        printf("Circular DMA (Linked List) started on channel %d\r\n", obj->rxChannel);
        printf("  Buffer: 0x%08lX, Size: %d\r\n", (uint32_t)obj->SDQueue, BUFFER_SIZE);
    }
    else
    {
        printf("ERROR: Failed to start Linked List DMA on channel %d\r\n", obj->rxChannel);
    }
}


bool PacketQueue_Push(FLEXCOM_DMA_OBJECT* obj)
{
    uint8_t chId = obj->headerIdx;
    uint32_t remaining1;
    uint32_t remaining2;
    uint16_t SDQueueHead;
    uint16_t SDQueueTail = obj->SDQueueTail;
    uint16_t availableBytes;
    uint16_t nextPckHead;
    uint16_t writeSlot;

    // Self-linked RX DMA에서는 CDA보다 CUBC(남은 길이) 기반 head 계산이 wrap 경계에서 안정적이다.
    do
    {
        remaining1 = XDMAC_REGS->XDMAC_CHID[obj->rxChannel].XDMAC_CUBC & XDMAC_CUBC_UBLEN_Msk;
        remaining2 = XDMAC_REGS->XDMAC_CHID[obj->rxChannel].XDMAC_CUBC & XDMAC_CUBC_UBLEN_Msk;
    } while (remaining1 != remaining2);

    SDQueueHead = (uint16_t)((BUFFER_SIZE - remaining1) % BUFFER_SIZE);

    if (SDQueueHead >= SDQueueTail)
    {
        availableBytes = SDQueueHead - SDQueueTail;
    }
    else
    {
        availableBytes = (BUFFER_SIZE - SDQueueTail) + SDQueueHead;
    }

    if (availableBytes < DATA_SIZE)
    {
        return false;
    }

    nextPckHead = (uint16_t)((FLEXCOM5PacketQueue.PckQHead + 1U) % BUFFER_SIZE);
    if (nextPckHead == FLEXCOM5PacketQueue.PckQTail)
    {
        return false;
    }

    // Zero-copy: 데이터는 복사하지 않고, 큐 슬롯에는 원본 위치 메타데이터만 저장한다.
    writeSlot = FLEXCOM5PacketQueue.PckQHead;
    FLEXCOM5PacketQueue.Pck[writeSlot][0] = chId;
    FLEXCOM5PacketQueue.Pck[writeSlot][1] = (uint8_t)(DATA_SIZE & 0xFFU);

    // [2..3] = SDQueue 시작 오프셋
    FLEXCOM5PacketQueue.Pck[writeSlot][2] = (uint8_t)(SDQueueTail & 0xFFU);
    FLEXCOM5PacketQueue.Pck[writeSlot][3] = (uint8_t)((SDQueueTail >> 8) & 0xFFU);

    obj->SDQueueTail = (uint16_t)((SDQueueTail + DATA_SIZE) % BUFFER_SIZE);
    FLEXCOM5PacketQueue.PckQHead = nextPckHead;
    obj->rxPacketCount++;

    // 패킷이 채워지면 즉시 TX DMA 시작 시도
    DMA_TX_init();
    return true;
}

void DMA_TX_init()
{
    uint16_t readSlot;
    uint8_t chId;
    uint16_t dataLen;
    uint16_t startOffset;
    FLEXCOM_DMA_OBJECT* srcObj;
    uint8_t* txSrc;
    uint16_t firstChunk;

    if (FLEXCOM5PacketQueue.PckQTail == FLEXCOM5PacketQueue.PckQHead)
    {
        return;
    }

    while (XDMAC_ChannelIsBusy(FLEXCOM_TX_XDMAC_CHANNEL))
    {
    }

    readSlot = FLEXCOM5PacketQueue.PckQTail;

    chId = FLEXCOM5PacketQueue.Pck[readSlot][0];
    dataLen = FLEXCOM5PacketQueue.Pck[readSlot][1];
    startOffset = (uint16_t)FLEXCOM5PacketQueue.Pck[readSlot][2]
        | ((uint16_t)FLEXCOM5PacketQueue.Pck[readSlot][3] << 8);

    srcObj = GetDmaObjByHeader(chId);
    if (srcObj == NULL)
    {
        FLEXCOM5PacketQueue.PckQTail = (uint16_t)((FLEXCOM5PacketQueue.PckQTail + 1U) % BUFFER_SIZE);
        return;
    }

    // 헤더 2바이트는 CPU로 먼저 송신 (payload는 zero-copy DMA)
    g_txHeader[0] = chId;
    g_txHeader[1] = (uint8_t)(dataLen & 0xFFU);
    while ((FLEXCOM5_REGS->FLEX_US_CSR & FLEX_US_CSR_TXRDY_Msk) == 0U) { }
    FLEXCOM5_REGS->FLEX_US_THR = g_txHeader[0];
    while ((FLEXCOM5_REGS->FLEX_US_CSR & FLEX_US_CSR_TXRDY_Msk) == 0U) { }
    FLEXCOM5_REGS->FLEX_US_THR = g_txHeader[1];

    txSrc = &srcObj->SDQueue[startOffset % BUFFER_SIZE];
    firstChunk = (uint16_t)(BUFFER_SIZE - (startOffset % BUFFER_SIZE));
    if (firstChunk > dataLen)
    {
        firstChunk = dataLen;
    }

    // Zero-copy payload DMA (wrap 구간은 다음 push에서 처리)
    SCB_CleanDCache_by_Addr((uint32_t*)txSrc, (int32_t)firstChunk);

    if (XDMAC_ChannelTransfer(
            FLEXCOM_TX_XDMAC_CHANNEL,
            txSrc,
            (const void*)&(FLEXCOM5_REGS->FLEX_US_THR),
            (size_t)firstChunk))
    {
        FLEXCOM5PacketQueue.PckQTail = (uint16_t)((FLEXCOM5PacketQueue.PckQTail + 1U) % BUFFER_SIZE);
    }
}
