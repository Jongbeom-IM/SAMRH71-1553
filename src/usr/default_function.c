#include "default_function.h"
#include "definitions.h"
#include "FLEXCOM_dma_types.h"

volatile bool SYSTEM_PAUSED = true;

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

}

static void PrintDmaObjectInfo(const char* name, const FLEXCOM_DMA_OBJECT* obj)
{
  printf("%s: header=0x%02X channel=%u rxReg=0x%08lX tail=%u\r\n",
    name,
    obj->headerIdx,
    (unsigned)obj->rxChannel,
    (unsigned long)obj->rxReg,
    obj->SDQueueTail);
}

void Button_Callback(PIO_PIN pin, uintptr_t context)
{
    (void)context;  // unused
    
    switch (pin)
    {
        case PIO_PIN_PC30:
            PrintDmaObjectInfo("FLEXCOM0DmaObj", &FLEXCOM0DmaObj);
            PrintDmaObjectInfo("FLEXCOM2DmaObj", &FLEXCOM2DmaObj);
            PrintDmaObjectInfo("FLEXCOM3DmaObj", &FLEXCOM3DmaObj);
            PrintDmaObjectInfo("FLEXCOM4DmaObj", &FLEXCOM4DmaObj);
            break;
        case PIO_PIN_PC31:
            break;
        default:
            break;
    }
}

void PlayCallback(PIO_PIN pin, uintptr_t context)
{
    (void)pin;
    (void)context;  // unused
    SYSTEM_PAUSED = !SYSTEM_PAUSED;
    printf("System %s\r\n", SYSTEM_PAUSED ? "Paused" : "Running");

    static uint8_t testData[2] = {0xFF, 0xFF};
    while (XDMAC_ChannelIsBusy(FLEXCOM_TX_XDMAC_CHANNEL)) {}
    SCB_CleanDCache_by_Addr((uint32_t*)testData, sizeof(testData));
    XDMAC_ChannelTransfer(FLEXCOM_TX_XDMAC_CHANNEL,
        (const void*)testData,
        (const void*)&(FLEXCOM5_REGS->FLEX_US_THR),
        sizeof(testData));
}