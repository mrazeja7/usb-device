#include "stm32f4xx_conf.h"
#include "lcd.h"
#include "usb_defs.h"
#include "enums.h"
#include <stdlib.h>
#include <stdio.h>
#include "delay.h" // TODO weird code
#include "lcd_func.h"
#include "usb_descriptors.h"
#include "inits.h"
#include "stm324xg_eval.h"

// global vars used in my experimental movement timing hack
uint8_t mouseready = 0;
uint8_t moving = 0;

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
    USB_OTG_FS->GRXFSIZ = 256U; // min 16, max 256  
    
    // ??? DIEPTXF0_HNPTXFSIZ is the only register I found with a zero in its name ???
    // http://www.disca.upv.es/aperles/arm_cortex_m3/llibre/st/STM32F439xx_User_Manual/stm32f4xx__hal__pcd__ex_8c_source.html - LINE 108, slightly changed
	USB_OTG_FS->DIEPTXF0_HNPTXFSIZ = (256U << 16);
	USB_OTG_FS->DIEPTXF0_HNPTXFSIZ |= 128U; // TX FIFO start address right after RX FIFO - USB_OTG_FS->GRXFSIZ
    
	//4.
    USB_OTG_OUT_ENDPOINT0->DOEPTSIZ |= USB_OTG_DOEPTSIZ_STUPCNT_0 | USB_OTG_DOEPTSIZ_STUPCNT_1; // 0b11
    
    USBD_FS->DCFG &= ~USB_OTG_DCFG_DAD;
	USBD_FS->DCFG |= USB_OTG_DCFG_DSPD;
    
    USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_USBRST;
//    displayText("USB reset complete", 18, 0);	
}

void usb_enum_done()
{
    // 29.17.5
    // enum speed is known as this is a USB_FS device
    // MPSIZ should be set to 00 for 64 bytes, therefore no need to touch it
  
    // 2
    USB_OTG_IN_ENDPOINT0->DIEPCTL |= ~(USB_OTG_DIEPCTL_MPSIZ & 0x3);
    USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE;
    displayText("ENUMDNE complete", 16, 0);	
}

// need to remember the latest bRequest (multiple SETUP packets sent back to back)
static __IO uint8_t lastbReq = 0x0;
static __IO uint8_t lastbReqLength = 0x0;
static __IO uint32_t lastbReqVal = 0x0;
static __IO uint32_t set = 0x0;

void sendEmptyIn()
{
    USB_OTG_IN_ENDPOINT0->DIEPCTL &= ~USB_OTG_DIEPCTL_TXFNUM;
    USB_OTG_IN_ENDPOINT0->DIEPTSIZ = (USB_OTG_DIEPTSIZ_PKTCNT & 0x80000);
    USB_OTG_IN_ENDPOINT0->DIEPCTL |= USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;
}

void sendEmptyOut()
{    
    USB_OTG_OUT_ENDPOINT0->DOEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & 0x40000) | 64U; // one packet of max size
    USB_OTG_OUT_ENDPOINT0->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
}

void sendData(volatile uint8_t * data, uint16_t len)
{
    USB_OTG_IN_ENDPOINT0->DIEPCTL &= ~USB_OTG_DIEPCTL_TXFNUM;
    // pg 1018, https://github.com/01org/zephyr/blob/master/ext/hal/st/stm32cube/stm32l4xx/drivers/src/stm32l4xx_ll_usb.c line ~646
    uint32_t dataLen = (len > lastbReqLength) ? lastbReqLength : len; // data is longer than requested
    USB_OTG_IN_ENDPOINT0->DIEPTSIZ = (USB_OTG_DIEPTSIZ_PKTCNT & 0x80000) | dataLen;
    USB_OTG_IN_ENDPOINT0->DIEPCTL |= USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;
    
    // fill TX FIFO here?
    // TX FIFO is uint32_t*, desc is byte array
    uint16_t steps = (dataLen + 3) / 4;
    volatile uint8_t *dataptr = data;
    for (uint16_t i = 0; i < steps; ++i) // 18 or 34 bytes, still works
    {
        USB_OTG_TX_DFIFO[i] = *((uint32_t*) dataptr);
        dataptr += 4;
    }
}

void sendStringDesc(uint8_t index)
{
    switch (index)
    {
        case 0:
            sendData(lang_descriptor, LANG_DESC_SIZE);
            break;
        case 1:
            sendData(vendor_string_descriptor, VENDOR_DESC_SIZE);
            break;
        case 2:
            sendData(product_string_descriptor, PRODUCT_DESC_SIZE);
            break;
        case 3:
            sendData(sn_string_descriptor, SN_DESC_SIZE);
            break;
        default:
            displayText("OTHER STRING DESC", 17, 0);
            break;
    }
}

void sendDescriptor()
{
    char str[20];
    uint8_t len;
    switch(lastbReqVal >> 8) // wValue
    {
        case device_desc:
//            sendData(razer_descriptor, DEV_DESC_SIZE);
            sendData(mouse_descriptor, DEV_DESC_SIZE);
            displayText("DEV DESC sent", 13, 0);
            break;
        case configuration_desc:
            sendData(config_descriptor_set, CONF_DESC_SET_SIZE);
            displayText("CONF DESC sent", 14, 0);
            break;
        case string_desc:
            displayText("STRING DESC", 11, 0);
            sendStringDesc(lastbReqVal & 0xFF);
            break;
//        case device_qualifier_desc:
//            displayText("DEV QUAL DESC", 13, 0);
//            sendData(qualifier_descriptor, QUALIFIER_DESC_SIZE);
//            break;
        case hid_report_descriptor:
            displayText("HID REP DESC", 12, 0);
            sendData(mouse_report_descriptor, HID_REPORT_DESC_SIZE);
            mouseready = 1;
            break;
        default:            
            len = sprintf(str, "OTHER DESC %0X", lastbReqVal >> 8);
            displayText((uint8_t*) str, len, 0);
            break;
    }
}

void setAddr(uint16_t val)
{
    USBD_FS->DCFG |= ((val & 0x7F) << 4);
    char str[20];
    uint8_t len = sprintf(str, "SET_ADDR %02X", val);
    displayText((uint8_t*) str, len, 0);
}

void initialize_inep1()
{
    USB_OTG_FS->DIEPTXF[0] = (256U << 16);
	USB_OTG_FS->DIEPTXF[0] |= 128U; // same thing as for in EP 0, except this one is periodic
    USBD_FS->DAINTMSK |= 0x2; // IEPM = bit 1 for endpoint 1
    // page 1041
    USB_OTG_IN_ENDPOINT1->DIEPCTL = 64U | USB_OTG_DIEPCTL_USBAEP 
                                        | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_EONUM_DPID 
                                        | USB_OTG_DIEPCTL_TXFNUM_0; // define a new TX_DFIFO - pg 1090 USB_OTG_TX_DFIFO1
}

void processSetup()
{   
    if(USB_OTG_OUT_ENDPOINT0->DOEPINT & USB_OTG_DOEPINT_STUP) 
        USB_OTG_OUT_ENDPOINT0->DOEPINT |= USB_OTG_DOEPINT_STUP;
  
    if (!set)
        return;
    
    char str[20];
    uint8_t len;
    
    switch (lastbReq)
    {
        case GET_DESCRIPTOR: // send descriptor at this point
            sendDescriptor();
            break;
        case SET_ADDRESS:
            setAddr(lastbReqVal);
            break;
        case SET_CONFIGURATION:
            len = sprintf(str, "SET CONF %02X", lastbReqVal);
            displayText((uint8_t*) str, len, 0);
            break;
    case SET_IDLE:    
            sendEmptyIn();
            initialize_inep1();
            // initialize endpoint 1 here
            break;
        default:
//            displayText("OTHER PROC", 10, 0);
//            len = sprintf(str, "PROC %02X %02X", lastbReq, lastbReqVal);
//            displayText((uint8_t*) str, len, 0);
            break;
    }
    
    // reset last request information as this one is done
    lastbReq = 0x0; 
    lastbReqVal = 0x0;
    lastbReqLength = 0x0;
    set = 0x0;
    
    if(USB_OTG_OUT_ENDPOINT0->DOEPINT & USB_OTG_DOEPINT_XFRC) 
        USB_OTG_OUT_ENDPOINT0->DOEPINT |= USB_OTG_DOEPINT_XFRC;
}

void receive_setup(volatile uint32_t *data)
{
    // https://www.beyondlogic.org/usbnutshell/usb6.shtml#SetupPacket
    uint8_t bmRequestType = (uint8_t) data[0];
    uint8_t bRequest = (uint8_t) (data[0] >> 8);
    uint16_t wValue = (uint16_t) (data[0] >> 16);
    uint16_t wIndex = (uint16_t) data[1];
    uint16_t wLength = (uint16_t) (data[1] >> 16);
    
    uint8_t recipient = bmRequestType & 0x1F; // D4..0
    uint8_t type = (bmRequestType & 0x60) >> 5; // D6,5
//    uint8_t direction = (bmRequestType & 0x80) >> 7; // D7
    
    char str[20];
    //uint8_t len = sprintf(str, "STP %02X %02X %02X %02X %u", recipient, type, direction, bRequest, wValue);
    uint8_t len = sprintf(str, "STP %02X %02X %02X %02X %02X", wLength, recipient, type, bRequest, wValue);
//    displayText((uint8_t*) str, len, 0);
  
    switch(recipient)
    {
        case device:
            switch(type)
            {
                case standard:
                    switch(bRequest)
                    {
                        case GET_DESCRIPTOR: // 0x6
                            lastbReq = GET_DESCRIPTOR;
                            lastbReqVal = wValue;
                            lastbReqLength = wLength;
                            set = 0x1;
                            break;
                        case SET_ADDRESS: // 0x5
                            lastbReq = SET_ADDRESS;
                            lastbReqVal = wValue;
                            set = 0x1;
                            sendEmptyIn();
                            break;    
                        case GET_STATUS: // 0x0
                            displayText("GET_STATUS", 10, 0);
                            break;
                        case SET_CONFIGURATION:
                            displayText("SET CONFIG", 10, 0);
                            sendEmptyIn();
                            break;
                        default:
                            displayText((uint8_t*) str, len, 0);
                            displayText("OTHER bREQ", 10, 0);
                            break;
                    }
                    break;
                default:
                    displayText("OTHER TYPE", 10, 0);
                    break;
            }
            break;            
        case interface: // 0x1
            switch (type)
            {
                case standard:
                    switch (bRequest)
                    {
                        case GET_DESCRIPTOR:
                            // wValue is 0x2200
                            // https://www.silabs.com/documents/public/application-notes/AN249.pdf pg 18 - 0x22 is HID descriptor
                            // also hid1_11.pdf section 7.1
                            lastbReq = GET_DESCRIPTOR;
                            lastbReqVal = wValue;
                            lastbReqLength = wLength;
                            set = 0x1;
                            break;
                    }
                    break;
                case class:
                    switch (bRequest)
                    {
                        case SET_IDLE: // SET_IDLE https://www.microchip.com/forums/m883877.aspx
                            lastbReq = SET_IDLE;
                            lastbReqVal = wValue;
                            lastbReqLength = wLength;
                            set = 0x1;
                            break;
                    }
                    break;  
                default:
                    displayText((uint8_t*) str, len, 0);
                    displayText("OTHER TYPE", 10, 0);
                    break;
            }
            break;
        case endpoint: // 0x2
            switch (type)
            {
                case standard:
                    switch (bRequest)
                    {
                        case CLEAR_FEATURE:
                            // do nothing?
                            break;
                    }
                    break;
                default:
                    displayText((uint8_t*) str, len, 0);
                    displayText("OTHER TYPE", 10, 0);
                    break;
            }
            break;
        default:
            displayText((uint8_t*) str, len, 0);
            displayText("OTHER RECP", 10, 0);
            break;
    }
    // http://www.usbmadesimple.co.uk/ums_5.htm mouse input report
}

void usb_receive()
{
    // 29.17.6
    
    // 1
    uint32_t grxstsp = USB_OTG_FS->GRXSTSP;
    // 2
    USB_OTG_FS->GINTMSK &= ~USB_OTG_GINTMSK_RXFLVLM;
    
    // 3, 4
    uint32_t bytecount = (grxstsp & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;
    uint8_t pktsts = (grxstsp & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos;
    uint8_t dpid = (grxstsp & USB_OTG_GRXSTSP_DPID) >> USB_OTG_GRXSTSP_DPID_Pos;
    uint8_t epnum = (grxstsp & USB_OTG_GRXSTSP_EPNUM);
    
    char str[16];
    uint8_t len = sprintf(str, "RXF %02X %02X %02X %02X", pktsts, bytecount, epnum, dpid);
//    displayText((uint8_t*) str, len, 0);
    
    if (bytecount > 0)
    {
        // RX FIFO is at USB_base + 0x1000, pg 958 in ref guide
        // https://community.st.com/s/question/0D50X00009Xkf0SSAR/usb-host-channel-interrupt-register-is-always-zero-after-a-transaction
        uint16_t wordcount = (bytecount + 3) / 4;
        volatile uint32_t *rxfifo = USB_OTG_RX_DFIFO;
        volatile uint32_t data[3];
        // 4b - setup packet pattern
        if (pktsts == PKTSTS_SETUP && bytecount == 0x008 && epnum == 0 && dpid == 0)
        {
            for (uint16_t i = 0; i < wordcount; ++i)
                data[i] = rxfifo[0];
            receive_setup(data);
        }
        else 
            displayText("OTHER DATA", 10, 0);
          
    }
    // 5
    USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
}

void sendExperimentalMovement(char direction)
{
    // https://visualgdb.com/tutorials/arm/stm32/timers/ timer interrupts - didn't get them working
    moving = 1;
    while ((USB_OTG_IN_ENDPOINT1->DIEPINT & USB_OTG_DIEPINT_TXFE	) == 0); // wait until TX fifo is empty
    Delay(5000); // wait some more
    USB_OTG_IN_ENDPOINT1->DIEPINT |= USB_OTG_DIEPINT_TXFE;
    USB_OTG_IN_ENDPOINT1->DIEPCTL |= USB_OTG_DIEPCTL_TXFNUM_0;
    USB_OTG_IN_ENDPOINT1->DIEPTSIZ = (USB_OTG_DIEPTSIZ_PKTCNT & 0x80000) | 3U;
    USB_OTG_IN_ENDPOINT1->DIEPCTL |= USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;

    int8_t state[4];
    switch (direction)
    {
        case 'l':
            state[1] = -2; // move to the left by 2 units
            break;
        case 'r':
            state[1] = 2; // move to the right by 2 units
            break;
        default:
            break;
    }
    
    USB_OTG_TX_DFIFO1[0] = *((uint32_t *)&state);
}

// adapted from STM32F4xx_StdPeriph_Examples\I2C\I2C_IOExpander
void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(TAMPER_BUTTON_EXTI_LINE) != RESET)
    {        
        displayText("TAMPER", 6, 0);
        sendExperimentalMovement('r');
        
        EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE);
    }
}

void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(WAKEUP_BUTTON_EXTI_LINE) != RESET)
    {        
        displayText("WAKEUP", 6, 0);
        sendExperimentalMovement('l');
        
        EXTI_ClearITPendingBit(WAKEUP_BUTTON_EXTI_LINE);
    }
}

void initializeButtons()
{
    STM_EVAL_PBInit(BUTTON_TAMPER, BUTTON_MODE_EXTI);
    STM_EVAL_PBInit(BUTTON_WAKEUP, BUTTON_MODE_EXTI);
}

void OTG_FS_IRQHandler(void) 
{
    // this insanity allows the code to wait a little bit (IRQ Handler fires 255 times) after the HID setup is 
    // completely ready (mouseready is 0 until then), then we can send mouse movement packets indefinitely
//    if ((!moving && mouseready != 0 && ++mouseready != 0) || moving) // small delay, then fire
//        sendExperimentalMovement();
    uint32_t gintsts = USB_OTG_FS->GINTSTS;
    uint32_t daint = USBD_FS->DAINT;
    if (gintsts & USB_OTG_GINTSTS_USBRST)
    {
        usb_reset();
    }
    
    if (gintsts & USB_OTG_GINTSTS_ENUMDNE)
    {    
        usb_enum_done();
    }
    
    if (gintsts & USB_OTG_GINTSTS_RXFLVL)
    {
        usb_receive();
    }
    
    // page 1043
    if (gintsts & USB_OTG_GINTSTS_OEPINT)
    {
        if (daint & USB_OTG_DAINT_OEPINT) // setup data transfer complete, handle last setup packet
        {            
            processSetup();
        }
    }
    
    if (gintsts & USB_OTG_GINTSTS_IEPINT)
    {
        if (daint & (USB_OTG_DAINT_IEPINT & 0x1)) // IN endpoint 0, pg 995
        {
            uint32_t iepint = USB_OTG_IN_ENDPOINT0->DIEPINT;
            
            if (iepint & USB_OTG_DIEPINT_XFRC) // transfer complete
            {
				USB_OTG_IN_ENDPOINT0->DIEPINT |= USB_OTG_DIEPINT_XFRC;
                sendEmptyOut();
//                displayText("IN0 IEPINT XFRC", 15, 0);
			}
            
			if (iepint & USB_OTG_DIEPINT_TOC) // timeout
            {
				USB_OTG_IN_ENDPOINT0->DIEPINT |= USB_OTG_DIEPINT_TOC;
//                displayText("IN0 IEPINT TOC", 14, 0);
			}
            
			if (iepint & USB_OTG_DIEPINT_TXFE) // TX FIFO empty
            {
				USB_OTG_IN_ENDPOINT0->DIEPINT |= USB_OTG_DIEPINT_TXFE;
//                displayText("IN0 IEPINT TXFE", 15, 0);
			}	
        }
        if (daint & (USB_OTG_DAINT_IEPINT & 0x2)) // IN endpoint 1
        {
            if(USB_OTG_IN_ENDPOINT1->DIEPINT & USB_OTG_DIEPINT_XFRC) 
            {
//                displayText("IN EP 1 XFRC", 12, 0);
                USB_OTG_IN_ENDPOINT1->DIEPINT |= USB_OTG_DIEPINT_XFRC;
            }
			
        }
    }
    
    return; 
} 

int main()
{
    sysclk_init();
    
    LCD_Init();
    LCD_SetFont(&Font16x24); 

    usb_init();
    Delay(500);
    usb_core_init();
    
    nvic_init();
    
    initializeButtons();    
    while(1);
//    return 0;
}
