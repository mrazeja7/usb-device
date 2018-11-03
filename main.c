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
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
    
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10, GPIO_AF_OTG1_FS); // ID
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource11, GPIO_AF_OTG1_FS); // Data-
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource12, GPIO_AF_OTG1_FS); // Data+
}

void usb_core_init() // 29.17.1
{
    // 1
    USB_OTG_FS->GAHBCFG |= USB_OTG_GAHBCFG_GINT | USB_OTG_GAHBCFG_PTXFELVL;
    USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_RXFLVL;
    
    // 2
    // AHB = 168, TRDT = 15
    // force device mode?
    USB_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_HNPCAP | USB_OTG_GUSBCFG_SRPCAP | USB_OTG_GUSBCFG_TOCAL | (USB_OTG_GUSBCFG_TRDT & 0xF);

    
    // 3
    USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_OTGINT | USB_OTG_GINTMSK_MMISM;
    
    // 4
    // if (USB_OTG_FS->GINTSTS_CMOD & 0b1) // host mode, else device mode
    
}

//void OTG_FS_IRQHandler(void) 
//{ 
//    if (OTG_FS_GINTSTS & ???) 
//    { 
//        OTG_FS_GINTSTS = ???; 
//        usb_reset(); 
//    } 
//    if (OTG_FS_GINTSTS & ???) 
//    { 
//        OTG_FS_GINTSTS = ???;
//        usb_enum_done(); 
//    } 
//    if (OTG_FS_GINTSTS & ???) 
//    { 
//        usb_receive(); 
//    } 
//    return; 
//}

int main()
{
    usb_init();
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    usb_core_init();
    
    while(!(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_USBRST));
        __NOP();
    
    return 0;
}
