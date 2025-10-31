#include "stm32f10x.h"
#include "LED.h"
#include "delay.h"

#define LED1_PIN    GPIO_Pin_12
#define LED2_PIN    GPIO_Pin_13  
#define LED3_PIN    GPIO_Pin_14
#define LED4_PIN    GPIO_Pin_15

static uint32_t led_speed_counter = 50;
static uint8_t led_current = 0;
static uint8_t led_direction = 0;
static uint8_t led_speed_level = 0;
static uint32_t loop_counter = 0;

void LED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);
}

void LED_UpdateSpeed(uint8_t speed_level) {
    led_speed_level = speed_level;
    
    switch(speed_level) {
        case 0:  // 500ms
            led_speed_counter = 50;  // 50 * 10ms = 500ms
            break;
        case 1:  // 1000ms
            led_speed_counter = 100; // 100 * 10ms = 1000ms
            break;
        case 2:  // 200ms
            led_speed_counter = 20;  // 20 * 10ms = 200ms
            break;
    }
    
    loop_counter = 0;
}

void LED_UpdateDirection(uint8_t direction) {
    led_direction = direction;
}

uint8_t LED_GetDirection(void) {
    return led_direction;
}

uint8_t LED_GetSpeedLevel(void) {
    return led_speed_level;
}

float LED_GetSpeedLevelForDisplay(void) {
    return (float)led_speed_level;
}

void LED_Run(void) {
    loop_counter++;
    
    if (loop_counter >= led_speed_counter) {
        loop_counter = 0; 
        GPIO_ResetBits(GPIOB, LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);
        if(led_current == 0) {
            GPIO_SetBits(GPIOB, LED1_PIN);
        }
        else if(led_current == 1) {
            GPIO_SetBits(GPIOB, LED2_PIN);
        }
        else if(led_current == 2) {
            GPIO_SetBits(GPIOB, LED3_PIN);
        }
        else if(led_current == 3) {
            GPIO_SetBits(GPIOB, LED4_PIN);
        }
        if (led_direction == 0) {
            led_current = (led_current + 1) % 4;
        } else {
            led_current = (led_current == 0) ? 3 : (led_current - 1);
        }
    }
}
