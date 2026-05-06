#ifndef IP1553_RT_H
#define IP1553_RT_H

#include <stdint.h>
#include <stdbool.h>
#include "definitions.h"

// *****************************************************************************
// Section: IP1553 RT Mode Configuration
// *****************************************************************************

/* Default RT Address (can be changed in IP1553_RT_Initialize) */
#define IP1553_DEFAULT_RT_ADDR          1

// *****************************************************************************
// Section: IP1553 RT Mode Functions
// *****************************************************************************

/**
 * @brief Initialize IP1553 in RT (Remote Terminal) mode
 * @param rtAddress RT address (1-30)
 */
void IP1553_RT_Initialize(uint8_t rtAddress);

/**
 * @brief Set data in TX buffer for a sub-address
 *        This data will be sent when BC requests it (RT -> BC)
 * @param subAddr Sub-address (0-31)
 * @param data Pointer to data array (16-bit words)
 * @param wordCount Number of words (1-32)
 */
void IP1553_RT_SetTxData(uint8_t subAddr, uint16_t* data, uint8_t wordCount);

/**
 * @brief Get received data from RX buffer for a sub-address
 *        Call this after ERX interrupt to get data from BC
 * @param subAddr Sub-address (0-31)
 * @param data Pointer to receive buffer (16-bit words)
 * @param wordCount Number of words to read (1-32)
 */
void IP1553_RT_GetRxData(uint8_t subAddr, uint16_t* data, uint8_t wordCount);

/**
 * @brief Reset RX buffer to receive new data
 * @param subAddr Sub-address (0-31)
 */
void IP1553_RT_ResetRxBuffer(uint8_t subAddr);

/**
 * @brief Process IP1553 events (call in main loop or ISR)
 * @return IP1553 interrupt status mask
 */
IP1553_INT_MASK IP1553_RT_ProcessEvents(void);

/**
 * @brief Get total RX transfer count
 * @return Number of successful receptions
 */
uint32_t IP1553_RT_GetRxCount(void);

/**
 * @brief Get total TX transfer count
 * @return Number of successful transmissions
 */
uint32_t IP1553_RT_GetTxCount(void);

/**
 * @brief Run RT mode test
 */
void IP1553_RT_Test(void);

/**
 * @brief Poll for events (call in main loop)
 */
void IP1553_RT_Poll(void);

/**
 * @brief Print current RT status
 */
void IP1553_RT_PrintStatus(void);

#endif /* IP1553_RT_H */
