#include "stm32f4xx_conf.h"
#include "lcd.h"
#include "usb_defs.h"
#include <stdlib.h>
#include <stdio.h>

void sysclk_init()
{
    // clock demo
    /* nastavime hodiny dle Clock tree v reference manual - Figure 9.
    *
    * nastaveni radeji overit, nejsem si zcela jist
    */
    RCC_HSEConfig(RCC_HSE_ON);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div4);
    RCC_PCLK2Config(RCC_HCLK_Div2);
    RCC_PLLConfig(RCC_PLLSource_HSE, 25, 336, 2, 7);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == 0) ;
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

void usb_init()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE);	
    
    // USB HW initialization
    GPIO_InitTypeDef GPIO_InitStructure;
//  GPIO_InitStructure.GPIO_Pin = ??? | ??? | ???; 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
//  GPIO_PinAFConfig(GPIOA, ???, GPIO_AF_OTG_FS); 
//  GPIO_PinAFConfig(GPIOA, ???, GPIO_AF_OTG_FS); 
//  GPIO_PinAFConfig(GPIOA, ???, GPIO_AF_OTG_FS);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource11, GPIO_AF_OTG1_FS); 
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource12, GPIO_AF_OTG1_FS);
}

int main()
{
    *( (volatile unsigned long *) USB_REG1) = 0x00;
    
    usb_init();
    
    NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_InitStructure.NVIC_IRQChannel = ???; 
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ???; 
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = ???; 
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ???; 
    NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    return 0;
}
