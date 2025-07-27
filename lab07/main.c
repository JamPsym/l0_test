#include "stm32l053xx.h"
#include <stm32l0xx.h>

int main()
{
    // IO bus
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN);
    // GPIO
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE5, GPIO_MODER_MODE5_0);
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODE4, GPIO_MODER_MODE4_0);

    SystemCoreClockUpdate();

    SysTick_Config(SystemCoreClock/1000);

    for(;;)
    {
    }
}

static int software_counter;
void SysTick_Handler()
{
    static int blink_clock = 0;
    if(software_counter - blink_clock > 1000){
        GPIOA->ODR ^= GPIO_ODR_OD5;
        GPIOB->ODR ^= GPIO_ODR_OD4;
        blink_clock = software_counter;
    }

    software_counter++;
}
