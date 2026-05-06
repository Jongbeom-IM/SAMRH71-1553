#ifndef DEFAULT_FUNCTION_H
#define DEFAULT_FUNCTION_H

#include <stdint.h>
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************

extern volatile bool SYSTEM_PAUSED;

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

/**
 * @brief Millisecond delay (busy-wait)
 * @param ms Delay time in milliseconds
 */
void delay_ms(uint32_t ms);

/**
 * @brief Microsecond delay (busy-wait)
 * @param us Delay time in microseconds
 */
void delay_us(uint32_t us);

/**
 * @brief Print system information and current state
 */
void Print_system_info(void);

/**
 * @brief Button interrupt callback
 * @param pin The pin that triggered the interrupt
 * @param context User-defined context (not used)
 */
void Button_Callback(PIO_PIN pin, uintptr_t context);

/**
 * @brief Play button interrupt callback to toggle pause state
 * @param pin The pin that triggered the interrupt
 * @param context User-defined context (not used)
 */
void PlayCallback(PIO_PIN pin, uintptr_t context);

#endif /* DEFAULT_FUNCTION_H */

