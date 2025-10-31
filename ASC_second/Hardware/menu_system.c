#include "menu_system.h"
#include "delay.h"
#include "key.h"
#include "stm32f10x.h"
#include "encoder.h"

// 前向声明
Menu MainMenu;
uint8_t display_dirty = 1;
float mode = 0.0f;
MenuItem MainItems[] = {
    {"MODE", NULL, NULL, &mode, 0.0f, 1.0f, 1.0f},
};

// 菜单定义
Menu MainMenu = {"Main Menu", MainItems, 1, NULL};

void ShowInteger(uint8_t Line, uint8_t Column, int32_t number) {
    int32_t temp = number;
    uint8_t digits = 0;
    uint8_t i;
    
    // 处理负数
    if (number < 0) {
        OLED_ShowChar(Line, Column, '-');
        temp = -temp;  // 取绝对值
        Column++;
    }
    
    // 计算位数
    if (temp == 0) {
        digits = 1;
    } else {
        int32_t n = temp;
        while (n > 0) {
            digits++;
            n /= 10;
        }
    }
    
    // 显示数字
    for (i = 0; i < digits; i++) {
        int32_t divisor = 1;
        for (uint8_t j = 0; j < digits - i - 1; j++) {
            divisor *= 10;
        }
        uint8_t digit = (temp / divisor) % 10;
        OLED_ShowChar(Line, Column + i, digit + '0');
    }
}

// 辅助函数：显示浮点数（保留1位小数）
void ShowFloat(uint8_t Line, uint8_t Column, float number) {
    // 处理负数
    int32_t integer_part;
    int32_t decimal_part;
    
    if (number < 0) {
        // 显示负号
        OLED_ShowChar(Line, Column, '-');
        Column++;
        
        // 取绝对值后计算整数和小数部分
        float abs_number = -number;
        integer_part = (int32_t)abs_number;
        decimal_part = (int32_t)((abs_number - integer_part) * 10 + 0.5f);
    } else {
        integer_part = (int32_t)number;
        decimal_part = (int32_t)((number - integer_part) * 10 + 0.5f);
    }
    
    // 确保小数部分在0-9范围内
    if (decimal_part >= 10) {
        decimal_part = 0;
        integer_part++;  // 四舍五入进位
    }
    
    // 显示整数部分
    ShowInteger(Line, Column, integer_part);
    
    // 计算小数点位置
    uint8_t pos = Column;
    
    // 处理整数部分为0的情况
    if (integer_part == 0) {
        pos += 1;  // 只显示一个0
    } else {
        // 计算整数部分的位数
        int32_t temp = integer_part;
        while (temp > 0) {
            pos++;
            temp /= 10;
        }
    }
    
    // 显示小数点和小数部分
    OLED_ShowChar(Line, pos, '.');
    OLED_ShowChar(Line, pos + 1, decimal_part + '0');
}
void display_menu(void) {
    OLED_Clear();
    
 // 显示当前模式 - 修正显示方式
    OLED_ShowString(1, 1, "MODE:");
    
    // 根据mode值显示不同的内容
    if (*(MainItems->parameter) == 0.0f) {
        OLED_ShowString(1, 6, "OFF");
    } else {
        OLED_ShowString(1, 6, "ON ");
    }
}

/**
 * @brief 处理菜单导航
 * @param key: 按键输入
 */
void process_parameter_edit(Key_TypeDef key) {
    if (key == KEY) {
        if (*(MainItems->parameter) == 0.0f) {
            *(MainItems->parameter) = 1.0f;
        } else {
            *(MainItems->parameter) = 0.0f;
        }
        display_dirty = 1;
    }
}

// 初始化
void Menu_Init(void) {
	
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);
    
	 // 设置中断优先级 - 确保LED定时不受干扰
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 设置EXTI中断优先级较低（让LED运行不受影响）
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;  // 编码器中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    // 初始化OLED
    OLED_Init();
    OLED_Clear();
    
    display_dirty = 1;
    
    // 显示初始菜单
    display_menu();
}

// 刷新显示
void Menu_RefreshDisplay(void) {
    if (display_dirty) {
        display_menu();
			  Delay_ms(50);
    }
}
