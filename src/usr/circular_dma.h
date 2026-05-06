/*******************************************************************************
  Circular DMA Header File (Polling-based)

  File Name:
    circular_dma.h

  Summary:
    Circular DMA for UART RX - No callbacks, polling-based
 *******************************************************************************/

#ifndef CIRCULAR_DMA_H
#define CIRCULAR_DMA_H

#include "FLEXCOM_dma_types.h"

// *****************************************************************************
// Section: DMA Functions
// *****************************************************************************

/**
 * @brief Circular DMA 초기화 및 시작
 * 
 * DMA가 rxBuffer를 순환하면서 계속 데이터 수신
 * 콜백 없음 - XDMAC 상태 레지스터 폴링으로 위치 파악
 */
void CircularDMA_RX_Init(FLEXCOM_DMA_OBJECT* obj);

/**
 * @brief SDQueue에서 DATA_SIZE 바이트를 패킷 큐로 푸시
 */
bool PacketQueue_Push(FLEXCOM_DMA_OBJECT* obj);

/**
 * @brief Packet Queue를 TX DMA로 내보내기 위한 초기화 자리
 */
void DMA_TX_init(void);

/**
 * @brief MemToMem DMA 초기화 (테스트용)
 * 
 * 메모리 간 복사 DMA 설정 - 디버그/테스트용으로 사용
 */
void MemToMemDMA_Init(void);


#endif /* CIRCULAR_DMA_H */
