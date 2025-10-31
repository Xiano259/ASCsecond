#ifndef __MENU_SYSTEM_H
#define __MENU_SYSTEM_H

#include "stm32f10x.h"
#include "OLED.h"
#include "key.h"
#include <stdio.h>
#include <string.h>


// 菜单项结构
typedef struct MenuItem {
    char name[16];
    struct Menu* submenu;
    void (*action)(void);
    float* parameter;
    float min_val;
    float max_val;
    float step;
} MenuItem;

// 菜单结构
typedef struct Menu {
    char title[16];
    MenuItem* items;
    uint8_t item_count;
    struct Menu* parent;
} Menu;


// 函数声明
void Menu_Init(void);
void Menu_RefreshDisplay(void);
void process_parameter_edit(Key_TypeDef key);
#endif

