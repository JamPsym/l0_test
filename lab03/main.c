#include "stm32l053xx.h"
#include "system_stm32l0xx.h"
#include <stdint.h>

#include <stm32l0xx.h>

#include <stm32l0xx_ll_utils.h>
#include <stm32l0xx_ll_bus.h>
#include <stm32l0xx_ll_gpio.h>
#include <stm32l0xx_ll_tim.h>

int main()
{

    SystemCoreClockUpdate();
    LL_Init1msTick(SystemCoreClock); 

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    LL_GPIO_InitTypeDef gpioa;
    LL_GPIO_StructInit(&gpioa);
    gpioa.Mode = LL_GPIO_MODE_ALTERNATE;
    gpioa.Pin = LL_GPIO_PIN_5;
    LL_GPIO_Init(GPIOA, &gpioa);

    // PA5 -- AF5 TIM2_CH1
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    gpioa.Alternate = LL_GPIO_AF_5;
    LL_GPIO_Init(GPIOA, &gpioa);

    LL_TIM_InitTypeDef tim2;
    LL_TIM_StructInit(&tim2);
    tim2.Prescaler = SystemCoreClock/1000 - 1;
    tim2.Autoreload = 500;
    LL_TIM_Init(TIM2, &tim2);

    LL_TIM_OC_InitTypeDef ch2;
    LL_TIM_OC_StructInit(&ch2);
    ch2.OCState = LL_TIM_OCSTATE_ENABLE;
    ch2.CompareValue = 0;
    ch2.OCMode = LL_TIM_OCMODE_TOGGLE;
    LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH1, &ch2);

    LL_TIM_EnableCounter(TIM2);

    for(;;)
    {
        // LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_5);
        // LL_mDelay(500);
    }

    return 0;
}
