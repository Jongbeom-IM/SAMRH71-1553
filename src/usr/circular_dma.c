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
static uint8_t g_txLinearized[DATA_SIZE];

typedef struct
{
    uint32_t rxNotEnoughCount;
    uint32_t rxOverwriteRiskCount;
    uint32_t queuePushCount;
    uint32_t queueFullDropCount;
    uint32_t txStartCount;
    uint32_t txBusyDeferCount;
    uint32_t txWrapLinearizedCount;
    uint32_t txInvalidChannelDropCount;
    uint32_t txDmaFailCount;
} CIRCULAR_DMA_DIAGNOSTICS;

static CIRCULAR_DMA_DIAGNOSTICS g_dmaDiag;

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
    uint16_t packetLen;
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

    if (availableBytes <= DATA_SIZE - 1) // 최소한 DATA SIZE바이트는 있어야 패킷으로 인정
    {
        g_dmaDiag.rxNotEnoughCount++;

        return false;
    }

    if (availableBytes >= (BUFFER_SIZE - DATA_SIZE))
    {
        g_dmaDiag.rxOverwriteRiskCount++;
    }
    // printf("[RX] Ready: ch=%u avail=%u (>= %d)\r\n", (unsigned)chId, (unsigned)availableBytes, DATA_SIZE);

    /* Wire format uses 1-byte LEN, so one packet carries up to 255 bytes. */
    packetLen = DATA_SIZE;

    nextPckHead = (uint16_t)((FLEXCOM1PacketQueue.PckQHead + 1U) % BUFFER_SIZE);
    if (nextPckHead == FLEXCOM1PacketQueue.PckQTail)
    {
        g_dmaDiag.queueFullDropCount++;
        return false;
    }

    // Zero-copy: 데이터는 복사하지 않고, 큐 슬롯에는 원본 위치 메타데이터만 저장한다.
    writeSlot = FLEXCOM1PacketQueue.PckQHead;
    FLEXCOM1PacketQueue.Pck[writeSlot][0] = chId;
    FLEXCOM1PacketQueue.Pck[writeSlot][1] = (uint8_t)packetLen;

    // [2..3] = SDQueue 시작 오프셋
    FLEXCOM1PacketQueue.Pck[writeSlot][2] = (uint8_t)(SDQueueTail & 0xFFU);
    FLEXCOM1PacketQueue.Pck[writeSlot][3] = (uint8_t)((SDQueueTail >> 8) & 0xFFU);

    obj->SDQueueTail = (uint16_t)((SDQueueTail + packetLen) % BUFFER_SIZE);
    FLEXCOM1PacketQueue.PckQHead = nextPckHead;
    obj->rxPacketCount++;
    g_dmaDiag.queuePushCount++;
    // printf("[QUEUE] Packet enqueued: slot=%u, len=%u, tail=%u->%u\r\n", (unsigned)writeSlot, (unsigned)packetLen, (unsigned)SDQueueTail, (unsigned)obj->SDQueueTail);
    // 패킷이 채워지면 즉시 TX DMA 시작 시도
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

    if (FLEXCOM1PacketQueue.PckQTail == FLEXCOM1PacketQueue.PckQHead)
    {
        return;
    }

    if (XDMAC_ChannelIsBusy(FLEXCOM_TX_XDMAC_CHANNEL))
    {
        g_dmaDiag.txBusyDeferCount++;
        return;
    }

    readSlot = FLEXCOM1PacketQueue.PckQTail;

    chId = FLEXCOM1PacketQueue.Pck[readSlot][0];
    dataLen = FLEXCOM1PacketQueue.Pck[readSlot][1];
    if (dataLen == 0U)
    {
        FLEXCOM1PacketQueue.PckQTail = (uint16_t)((FLEXCOM1PacketQueue.PckQTail + 1U) % BUFFER_SIZE);
        return;
    }

    startOffset = (uint16_t)FLEXCOM1PacketQueue.Pck[readSlot][2] | ((uint16_t)FLEXCOM1PacketQueue.Pck[readSlot][3] << 8);

    srcObj = GetDmaObjByHeader(chId);
    if (srcObj == NULL)
    {
        g_dmaDiag.txInvalidChannelDropCount++;
        FLEXCOM1PacketQueue.PckQTail = (uint16_t)((FLEXCOM1PacketQueue.PckQTail + 1U) % BUFFER_SIZE);
        return;
    }

    // Wire header: [0x55][ID:1B][LEN:1B][0x00]
    g_txHeader[0] = 0x55U;
    g_txHeader[1] = chId;
    g_txHeader[2] = (uint8_t)dataLen;
    g_txHeader[3] = 0x00U;
    for (uint8_t hi = 0U; hi < HEADER_SIZE; hi++)
    {
        while ((FLEXCOM1_REGS->FLEX_US_CSR & FLEX_US_CSR_TXRDY_Msk) == 0U) { }
        FLEXCOM1_REGS->FLEX_US_THR = g_txHeader[hi];
    }

    txSrc = &srcObj->SDQueue[startOffset % BUFFER_SIZE];
    firstChunk = (uint16_t)(BUFFER_SIZE - (startOffset % BUFFER_SIZE));
    if (firstChunk > dataLen)
    {
        firstChunk = dataLen;
    }

    g_dmaDiag.txStartCount++;

    if (firstChunk < dataLen)
    {
        uint16_t secondChunk = (uint16_t)(dataLen - firstChunk);

        SCB_InvalidateDCache_by_Addr((uint32_t*)txSrc, (int32_t)firstChunk);
        memcpy(g_txLinearized, txSrc, firstChunk);
        SCB_InvalidateDCache_by_Addr((uint32_t*)srcObj->SDQueue, (int32_t)secondChunk);
        memcpy(&g_txLinearized[firstChunk], srcObj->SDQueue, secondChunk);
        SCB_CleanDCache_by_Addr((uint32_t*)g_txLinearized, (int32_t)dataLen);

        g_dmaDiag.txWrapLinearizedCount++;

        if (XDMAC_ChannelTransfer(
                FLEXCOM_TX_XDMAC_CHANNEL,
                g_txLinearized,
                (const void*)&(FLEXCOM1_REGS->FLEX_US_THR),
                (size_t)dataLen))
        {
            FLEXCOM1PacketQueue.PckQTail = (uint16_t)((FLEXCOM1PacketQueue.PckQTail + 1U) % BUFFER_SIZE);
        }
        else
        {
            g_dmaDiag.txDmaFailCount++;
            printf("[TX] DMA FAILED: from ch%u\r\n", (unsigned)chId);
        }
    }
    else
    {
        if (XDMAC_ChannelTransfer(
                FLEXCOM_TX_XDMAC_CHANNEL,
                txSrc,
                (const void*)&(FLEXCOM1_REGS->FLEX_US_THR),
                (size_t)dataLen))
        {
            FLEXCOM1PacketQueue.PckQTail = (uint16_t)((FLEXCOM1PacketQueue.PckQTail + 1U) % BUFFER_SIZE);
        }
        else
        {
            g_dmaDiag.txDmaFailCount++;
            printf("[TX] DMA FAILED: from ch%u\r\n", (unsigned)chId);
        }
    }
    
}

void CircularDMA_PrintDiagnostics(void)
{
    uint32_t txCc = (uint32_t)XDMAC_ChannelSettingsGet(FLEXCOM_TX_XDMAC_CHANNEL);
    uint32_t txPerid = (txCc & XDMAC_CC_PERID_Msk) >> XDMAC_CC_PERID_Pos;
    uint32_t txDsync = (txCc & XDMAC_CC_DSYNC_Msk) >> XDMAC_CC_DSYNC_Pos;
    uint32_t txGsBit = (XDMAC_REGS->XDMAC_GS & (XDMAC_GS_ST0_Msk << (uint32_t)FLEXCOM_TX_XDMAC_CHANNEL)) ? 1U : 0U;

    printf("\r\n=== Circular DMA Diagnostics ===\r\n");
    printf("queue pushes=%lu fullDrops=%lu\r\n",
        (unsigned long)g_dmaDiag.queuePushCount,
        (unsigned long)g_dmaDiag.queueFullDropCount);
    printf("rx notEnough=%lu overwriteRisk=%lu\r\n",
        (unsigned long)g_dmaDiag.rxNotEnoughCount,
        (unsigned long)g_dmaDiag.rxOverwriteRiskCount);
    printf("tx starts=%lu busyDefers=%lu wrapLinearized=%lu dmaFails=%lu invalidDrops=%lu\r\n",
        (unsigned long)g_dmaDiag.txStartCount,
        (unsigned long)g_dmaDiag.txBusyDeferCount,
        (unsigned long)g_dmaDiag.txWrapLinearizedCount,
        (unsigned long)g_dmaDiag.txDmaFailCount,
        (unsigned long)g_dmaDiag.txInvalidChannelDropCount);
    printf("tx ch=%u busy=%u gs=%lu perid=%lu dsync=%lu\r\n",
        (unsigned)FLEXCOM_TX_XDMAC_CHANNEL,
        XDMAC_ChannelIsBusy(FLEXCOM_TX_XDMAC_CHANNEL) ? 1U : 0U,
        (unsigned long)txGsBit,
        (unsigned long)txPerid,
        (unsigned long)txDsync);
    printf("FLEXCOM0 packets=%lu overruns=%lu parseErrors=%lu\r\n",
        (unsigned long)FLEXCOM0DmaObj.rxPacketCount,
        (unsigned long)FLEXCOM0DmaObj.overrunCount,
        (unsigned long)FLEXCOM0DmaObj.parseErrorCount);
    printf("FLEXCOM2 packets=%lu overruns=%lu parseErrors=%lu\r\n",
        (unsigned long)FLEXCOM2DmaObj.rxPacketCount,
        (unsigned long)FLEXCOM2DmaObj.overrunCount,
        (unsigned long)FLEXCOM2DmaObj.parseErrorCount);
    printf("FLEXCOM3 packets=%lu overruns=%lu parseErrors=%lu\r\n",
        (unsigned long)FLEXCOM3DmaObj.rxPacketCount,
        (unsigned long)FLEXCOM3DmaObj.overrunCount,
        (unsigned long)FLEXCOM3DmaObj.parseErrorCount);
    printf("FLEXCOM4 packets=%lu overruns=%lu parseErrors=%lu\r\n",
        (unsigned long)FLEXCOM4DmaObj.rxPacketCount,
        (unsigned long)FLEXCOM4DmaObj.overrunCount,
        (unsigned long)FLEXCOM4DmaObj.parseErrorCount);
}
