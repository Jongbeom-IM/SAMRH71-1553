#include "default_function.h"
#include "definitions.h"

// CPU_CLOCK_FREQUENCY = 100MHz
#define CYCLES_PER_LOOP 4U

void delay_ms(uint32_t ms)
{
    volatile uint32_t count = (CPU_CLOCK_FREQUENCY / 1000U / CYCLES_PER_LOOP) * ms;
    while (count--)
    {
        // busy wait
    }
}

void delay_us(uint32_t us)
{
    volatile uint32_t count = (CPU_CLOCK_FREQUENCY / 1000000U / CYCLES_PER_LOOP) * us;
    while (count--)
    {
        // busy wait
    }
}

void Print_system_info(void)
{
    printf("\r\n========================================\r\n");
    printf("  SAMRH71 EV Kit is Powered\r\n");
    printf("  Version: 0.1.0\r\n");
    printf("  Build Date: %s %s\r\n", __DATE__, __TIME__);
    printf("========================================\r\n");
    switch (MODE)
    {
        case 1:
            printf("  Normal Mode\r\n");
            break;
        case 2:
            printf("  1553 Test Mode\r\n");
            break;
        case 3:
            printf("  Mode 3\r\n");
            break;
        default:
            break;
    }
    printf("========================================\r\n");
}

void Button_Callback(PIO_PIN pin, uintptr_t context)
{
    (void)context;  // unused
    
    switch (pin)
    {
        case PIO_PIN_PC29:
            MODE = 1;
            break;
        case PIO_PIN_PC30:
            MODE = 2;
            break;
        case PIO_PIN_PC31:
            MODE = 3;
            break;
        default:
            break;
    }
    
    PIO_PinInterruptDisable(PIO_PIN_PC29);
    PIO_PinInterruptDisable(PIO_PIN_PC30);
    PIO_PinInterruptDisable(PIO_PIN_PC31);
}