#include "stm32f4xx_conf.h"
#include "lcd.h"
#include "usb_defs.h"
#include <stdlib.h>
#include <stdio.h>
#include "delay.h" // TODO weird code
#include "lcd_func.h"


void sysclk_init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;

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

    /* kazdou periferii je treba pripojit na sbernici (napajet), v nasem pripade
    * jsou to GPIOC, GPIOG a GPIOI
    */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOI, ENABLE);

    /* nastavime kazdy pin pro led jako vystupni - push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOI, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
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

void usb_core_init() 
{
    // 29.17.1
    // 1
    USB_OTG_FS->GAHBCFG |= USB_OTG_GAHBCFG_GINT | USB_OTG_GAHBCFG_PTXFELVL;
    USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_RXFLVL;
    
    // 2
    // AHB = 168, TRDT = 15
    // force device mode?
    USB_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_HNPCAP | USB_OTG_GUSBCFG_FDMOD | USB_OTG_GUSBCFG_SRPCAP | USB_OTG_GUSBCFG_TOCAL | (USB_OTG_GUSBCFG_TRDT & 0xF);
    
    // 3
    USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_OTGINT | USB_OTG_GINTMSK_MMISM;
    
	// 29.17.3
    // 1
    USBD_FS->DCFG |= (USB_OTG_DCFG_DSPD & 0x3); // Full speed
    
    // 2
	USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_ESUSPM 
      | USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_SOFM | USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_IEPINT | USB_OTG_GINTMSK_OEPINT;
    
    // 3
    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_PWRDWN | USB_OTG_GCCFG_VBUSBSEN;
    
    //if (USB_OTG_FS->GINTSTS_CMOD & 0b1) // host mode, else device mode    
    
    // 4
    while(!(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_USBRST));
}

void usb_reset()
{
    // 29.17.5
    
    // 1
    USB_OTG_OUT_ENDPOINT0->DOEPCTL   |= USB_OTG_DOEPCTL_SNAK;
    USB_OTG_OUT_ENDPOINT(1)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
    USB_OTG_OUT_ENDPOINT(2)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
    USB_OTG_OUT_ENDPOINT(3)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
    
    // 2
    USBD_FS->DAINTMSK |= (0x1 | (0x1 << 16) ); // IEPM = bit 0, OEPM = bit 16
    USBD_FS->DOEPMSK |= USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM;
    USBD_FS->DIEPMSK |= USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_TOM;
    
    // 3
    USB_OTG_FS->GRXFSIZ = 256; // min 16, max 256  
    
    // ??? DIEPTXF0_HNPTXFSIZ is the only register I found with a zero in its name ???
    // http://www.disca.upv.es/aperles/arm_cortex_m3/llibre/st/STM32F439xx_User_Manual/stm32f4xx__hal__pcd__ex_8c_source.html - LINE 108, slightly changed
	USB_OTG_FS->DIEPTXF0_HNPTXFSIZ = (256 << USB_OTG_TX0FD_Pos);
	USB_OTG_FS->DIEPTXF0_HNPTXFSIZ |= 256; // what is this?
    
	//4.
    USB_OTG_OUT_ENDPOINT0->DOEPTSIZ |= USB_OTG_DOEPTSIZ_STUPCNT_0 | USB_OTG_DOEPTSIZ_STUPCNT_1; // 0b11
    displayText("USB reset complete", 18, 0);	
}

void OTG_FS_IRQHandler(void) 
{ 
//    if (OTG_FS_GINTSTS & ???) 
//    { 
//        OTG_FS_GINTSTS = ???; 
//        //usb_reset(); 
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
    
    if (USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_USBRST)
    {
        displayText("USBRST", 6, 0);
        // reset caught
        usb_reset();
    }
    if (USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_ENUMDNE)
    {
        displayText("ENUMDNE", 7, 0);
        // determine enumeration speed
        //uint32_t spd = USB_OTG_DSTS_ENUMSPD;
        while (1);
    }
    
    return; 
} 

int main()
{
    sysclk_init();
    
    LCD_Init();
    LCD_SetFont(&Font16x24); 
    
    displayText("LCD init done", 13, 1);

    usb_init();
    Delay(500);
    usb_core_init();
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    return 0;
}
