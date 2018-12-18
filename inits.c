#include "stm32f4xx_conf.h"
#include "stm324xg_eval_ioe.h"
#include "usb_defs.h"
#include "inits.h"

void sysclkInit()
{
    GPIO_InitTypeDef GPIO_InitStructure;

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

void usbInit()
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

void usbCoreInit() 
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
    //USBD_FS->DCFG &= ~USB_OTG_DCFG_DAD; // reset device address - set address fails otherwise
    
    // 2
	USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_ESUSPM 
      | USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_SOFM | USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_IEPINT | USB_OTG_GINTMSK_OEPINT;
    
    // 3
    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_PWRDWN | USB_OTG_GCCFG_VBUSBSEN;
    
    //if (USB_OTG_FS->GINTSTS_CMOD & 0b1) // host mode, else device mode    
    
    // 4
    while(!(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_USBRST));
}

void nvicInit()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void initializeTimer()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 40000;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 19999;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
    TIM_Cmd(TIM2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void enableTimerInterrupt()
{
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}

void initializeButtons()
{
    STM_EVAL_PBInit(BUTTON_WAKEUP, BUTTON_MODE_GPIO);
    IOE_Config();
}
