/*******************************************************************************
  UART DMA Types Header File (Polling-based, No DMA Callbacks)

  Company:
    Microchip Technology Inc.

  File Name:
    uart_dma_types.h

  Summary:
    Type definitions for polling-based circular DMA with state machine parser

  Description:
    - DMA: Peripheral → Memory 매핑만 (콜백 없음)
    - 메인 루프에서 DMA 위치 폴링
    - 스테이트 머신으로 패킷 파싱
    - Circular Queue로 완성된 패킷 관리
 *******************************************************************************/

#ifndef UART_DMA_TYPES_H
#define UART_DMA_TYPES_H

#include "definitions.h"
#include "peripheral/xdmac/plib_xdmac.h"

// *****************************************************************************
// Section: Constants
// *****************************************************************************

#define BUFFER_SIZE          2048          // 버퍼 크기 (circular)
#define HEADER_SIZE          4             // 패킷 메타데이터: ID(1B) + LEN(1B) + SDQueue offset(2B)
#define DATA_SIZE              255           // 패킷당 최대 페이로드 바이트 (uint8_t LEN 필드에 맞게 255)

// *****************************************************************************
// Section: XDMAC Channel Definitions
// *****************************************************************************

#define FLEXCOM0_RX_XDMAC_CHANNEL      XDMAC_CHANNEL_0
#define FLEXCOM2_RX_XDMAC_CHANNEL      XDMAC_CHANNEL_1
#define FLEXCOM3_RX_XDMAC_CHANNEL      XDMAC_CHANNEL_2
#define FLEXCOM4_RX_XDMAC_CHANNEL      XDMAC_CHANNEL_3
#define FLEXCOM_TX_XDMAC_CHANNEL       XDMAC_CHANNEL_4

// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************

// UART DMA 객체 (RX 전용)
typedef struct {
    // Channel ID for framed packet header
    uint8_t  headerIdx;
    uint16_t dataIdx;

    // RX circular DMA descriptor (self-linked)
    XDMAC_DESCRIPTOR_VIEW_0 rxDescriptor __attribute__((aligned(4)));
      
    // ========================================
    // Packet Circular Queue (완성된 패킷)
    // ========================================
    uint8_t SDQueue[BUFFER_SIZE] __attribute__((aligned(32)));
    uint16_t SDQueueTail;       // 다음 읽기 위치

    // ========================================
    // DMA/UART 설정
    // ========================================
    XDMAC_CHANNEL rxChannel;
    const void*   rxReg;        // UART RHR 주소

    // ========================================
    // 통계
    // ========================================
    uint32_t rxByteCount;       // 총 수신 바이트
    uint32_t rxPacketCount;     // 완성된 패킷 수
    uint32_t overrunCount;      // 버퍼 오버런 횟수
    uint32_t parseErrorCount;   // 파싱 에러 횟수
} FLEXCOM_DMA_OBJECT;


typedef struct{
    uint8_t Pck[BUFFER_SIZE][HEADER_SIZE]; // 패킷 헤더 저장 (채널 ID, 데이터 길이 등)
    uint16_t PckQHead;
    uint16_t PckQTail;
} PACKET_QUEUE;
// *****************************************************************************
// Section: Global Variables (extern declarations)
// *****************************************************************************

extern FLEXCOM_DMA_OBJECT FLEXCOM0DmaObj;
extern FLEXCOM_DMA_OBJECT FLEXCOM2DmaObj;
extern FLEXCOM_DMA_OBJECT FLEXCOM3DmaObj;
extern FLEXCOM_DMA_OBJECT FLEXCOM4DmaObj;
extern PACKET_QUEUE FLEXCOM1PacketQueue;

#endif /* UART_DMA_TYPES_H */
