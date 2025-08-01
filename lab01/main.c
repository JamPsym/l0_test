#include "stm32l053xx.h"
#include "system_stm32l0xx.h"
#include <stm32l0xx.h>

#include <stdint.h>

static void gpio_init(void);
static void system_init(void);
static void exti_init(void);
static void rled_hw_toggle_init(int);
static void rled_hw_pwm_init(int);
static void gled_hw_pwm_init(int);
static void rled_pwm(uint8_t pwm);
static void gled_pwm(uint8_t pwm);

int main()
{
    // system_init();
    gpio_init();
    exti_init();

    rled_hw_toggle_init(500); 

    for(;;)
    {
    }
}

// 1000*powf(n, 2.2f) + 0.5;
static const uint16_t gamma_table[101] = {
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 8, 9, 11, 12, 14, 15, 17,
    19, 21, 23, 25, 28, 30, 33, 36, 39, 42, 45, 49, 52, 56, 60, 64, 68, 73, 77, 82, 87,
    92, 98, 103, 109, 115, 121, 128, 134, 141, 148, 155, 163, 170, 178, 186, 195, 203,
    212, 221, 231, 240, 250, 260, 271, 281, 292, 304, 315, 327, 339, 351, 364, 377, 390,
    404, 418, 432, 446, 461, 476, 492, 508, 524, 540, 557, 574, 592, 610, 628, 647, 666,
    685, 705, 725, 1000
};

// Fast integer-only gamma correction using lookup table
static uint16_t gamma_correct_pwm_fast(uint8_t linear_brightness) {
    // Clamp input to valid range
    if (linear_brightness > 100) {
        linear_brightness = 100;
    }
    
    return gamma_table[linear_brightness];
}

static void rled_pwm(uint8_t pwm) {
    TIM2->CCR1 = gamma_correct_pwm_fast(pwm);
}

static void gled_pwm(uint8_t pwm) {
    TIM22->CCR1 = gamma_correct_pwm_fast(pwm);
}


static void rled_hw_pwm_init(int pwm)
{
    // pwm
    // PA5 -- AF5 TIM2_CH1
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE5, 0x2 << GPIO_MODER_MODE5_Pos);
    MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL5, 5 << GPIO_AFRL_AFSEL5_Pos); // AF5

    // tim2 config test
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);

    TIM2->PSC = SystemCoreClock/1000000 - 1; // 1ms tick
    TIM2->ARR = 1000; 
    // TIM2_CH1 OC mode
    TIM2->CCR1 = pwm; // period
    CLEAR_BIT(TIM2->CCMR1, TIM_CCMR1_CC1S); // output mode
    MODIFY_REG(TIM2->CCMR1, TIM_CCMR1_OC1M, (0b110 << TIM_CCMR1_OC1M_Pos)); // pwm 1 mode

    SET_BIT(TIM2->CCER, TIM_CCER_CC1E);
    SET_BIT(TIM2->CR1, TIM_CR1_CEN);

}

static void gled_hw_pwm_init(int pwm)
{
    // PB4 -- AF4 TIM22_CH1
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODE4, 0x2 << GPIO_MODER_MODE4_Pos);
    MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL4, 4 << GPIO_AFRL_AFSEL4_Pos); // AF4

    // tim22 config
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM22EN);

    TIM22->PSC = SystemCoreClock/1000000 - 1;
    TIM22->ARR = 1000;
    // TIM22_CH1 OC mode
    TIM22->CCR1 = pwm; // period
    CLEAR_BIT(TIM22->CCMR1, TIM_CCMR1_CC1S); // output mode
    MODIFY_REG(TIM22->CCMR1, TIM_CCMR1_OC1M, (0b110 << TIM_CCMR1_OC1M_Pos)); // pwm 1 mode

    SET_BIT(TIM22->CCER, TIM_CCER_CC1E);
    SET_BIT(TIM22->CR1, TIM_CR1_CEN);
}

static void rled_hw_toggle_init(int ms)
{
    // toggle
    // PA5 -- AF5 TIM2_CH1
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE5, 0x2 << GPIO_MODER_MODE5_Pos);
    MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL5, 5 << GPIO_AFRL_AFSEL5_Pos); // AF5

    // tim2 config test
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);

    TIM2->PSC = SystemCoreClock/1000 - 1; // 1ms tick
    TIM2->ARR = ms; 
    // TIM2_CH1 OC mode
    TIM2->CCR1 = 0; // period
    CLEAR_BIT(TIM2->CCMR1, TIM_CCMR1_CC1S); // output mode
    MODIFY_REG(TIM2->CCMR1, TIM_CCMR1_OC1M, (0x3 << TIM_CCMR1_OC1M_Pos)); // toggle mode

    SET_BIT(TIM2->CCER, TIM_CCER_CC1E);
    SET_BIT(TIM2->CR1, TIM_CR1_CEN);
}

static int software_counter;
void SysTick_Handler()
{
    static int blink_clock = 0;
    if(software_counter - blink_clock > 10){
        static uint8_t dpwm = 100;
        static int8_t dir = -1;

        rled_pwm(dpwm);
        gled_pwm(100-dpwm);

        dpwm += dir;

        if (dpwm > 100) { // This will be true for 101 (from 100+1) and 255 (from 0-1)
             dir = -dir;
             if (dir == 1) { 
                 dpwm = 0; 
             } else { 
                 dpwm = 100; 
             }
        }

        blink_clock = software_counter;
    }

    software_counter++;
}

void EXTI0_1_IRQHandler()
{
    GPIOB->ODR ^= GPIO_ODR_OD4;

    SET_BIT(EXTI->PR, EXTI_PR_PIF0);
}

static void gpio_init(void)
{
    // IO bus
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN);
    // GPIO
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE5, GPIO_MODER_MODE5_0);
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODE4, GPIO_MODER_MODE4_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE0, 0);
}

static void system_init(void)
{
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock/1000);
}

static void exti_init(void)
{
    // CFG
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
    CLEAR_BIT(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI0);

    SET_BIT(EXTI->FTSR, EXTI_FTSR_FT0);
    SET_BIT(EXTI->IMR, EXTI_IMR_IM0);
    NVIC_EnableIRQ(EXTI0_1_IRQn);
}

