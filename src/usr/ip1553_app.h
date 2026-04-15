#ifndef IP1553_APP_H
#define IP1553_APP_H

#include <stdint.h>
#include <stdbool.h>
#include "definitions.h"

// *****************************************************************************
// Section: IP1553 Application Configuration
// *****************************************************************************

/* RT (Remote Terminal) Address to communicate with */
#define IP1553_TARGET_RT_ADDR           1

/* Sub-address for data transfer */
#define IP1553_DATA_SUBADDR             1

/* Number of data words to transfer (max 32) */
#define IP1553_DATA_WORD_COUNT          8

// *****************************************************************************
// Section: IP1553 Application Functions
// *****************************************************************************

/**
 * @brief Initialize IP1553 BC mode with buffers
 */
void IP1553_App_Initialize(void);

/**
 * @brief Send data to Remote Terminal (BC -> RT)
 * @param rtAddr RT address (1-30)
 * @param subAddr Sub-address (1-30)
 * @param data Pointer to data array (16-bit words)
 * @param wordCount Number of words to send (1-32)
 * @return true if command sent successfully
 */
bool IP1553_App_SendToRT(uint8_t rtAddr, uint8_t subAddr, uint16_t* data, uint8_t wordCount);

/**
 * @brief Receive data from Remote Terminal (RT -> BC)
 * @param rtAddr RT address (1-30)
 * @param subAddr Sub-address (1-30)
 * @param data Pointer to receive buffer (16-bit words)
 * @param wordCount Number of words to receive (1-32)
 * @return true if command sent successfully
 */
bool IP1553_App_ReceiveFromRT(uint8_t rtAddr, uint8_t subAddr, uint16_t* data, uint8_t wordCount);

/**
 * @brief Send Mode Command to RT
 * @param rtAddr RT address (1-30)
 * @param modeCmd Mode command code
 * @return true if command sent successfully
 */
bool IP1553_App_SendModeCommand(uint8_t rtAddr, IP1553_MODE_CMD modeCmd);

/**
 * @brief Check and handle IP1553 transfer status
 * @return IP1553 interrupt status
 */
IP1553_INT_MASK IP1553_App_CheckStatus(void);

/**
 * @brief Test IP1553 communication (loopback or RT test)
 */
void IP1553_App_Test(void);

#endif /* IP1553_APP_H */
