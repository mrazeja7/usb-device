/* USB descriptor definitions */
#ifndef __USB_DESCS_H
#define __USB_DESCS_H

#define DEV_DESC_SIZE 18U
#define CONF_DESC_SET_SIZE 34U
#define LANG_DESC_SIZE 4U
#define VENDOR_DESC_SIZE 14U
#define PRODUCT_DESC_SIZE 16U
#define SN_DESC_SIZE 10U
#define QUALIFIER_DESC_SIZE 10U

uint8_t device_descriptor[DEV_DESC_SIZE] = // 18 bytes
{
    18U, // bLength
    0x1, // bDescriptorType
    0x01,0x10, // bcdUSB
//    0x0,0x2, // USB 2.0
    0x0, // bDeviceClass
    0x0, // bDeviceSubClass
    0x0, // bDeviceProtocol
    64U, // bMaxPacketSize0      
    0x04,0x83, // idVendor - https://www.the-sz.com/products/usbid/index.php?v=&p=&n=STMicroelectronics
    0x12,0x34, // idProduct - don't care?
    0x0,0x1, // bcdDevice - don't care?
    0x1, // iManufacturer - no strings yet
    0x2, // iProduct - no strings yet
    0x3, // iSerialNumber - no strings yet
    0x1 // bNumConfigurations
};

uint8_t mouse_descriptor[DEV_DESC_SIZE] = // generic mouse - https://github.com/obdev/v-usb/blob/master/usbdrv/USB-IDs-for-free.txt
{
    18U, // bLength
    0x1, // bDescriptorType
//    0x01,0x10, // bcdUSB
    0x0,0x2, // USB 2.0
    0x3, // bDeviceClass
    0x0, // bDeviceSubClass
    0x0, // bDeviceProtocol
    64U, // bMaxPacketSize0      
    0xDA,0x27, // idVendor
    0xC0,0x16, // idProduct
    0x0,0x1, // bcdDevice
    0x1, // iManufacturer - no strings yet
    0x2, // iProduct - no strings yet
    0x3, // iSerialNumber - no strings yet
    0x1 // bNumConfigurations
};

uint8_t razer_descriptor[DEV_DESC_SIZE] = // Razer Deathadder
{
    18U, // bLength
    0x1, // bDescriptorType
//    0x01,0x10, // bcdUSB
    0x0,0x2, // USB 2.0
    0x3, // bDeviceClass
    0x0, // bDeviceSubClass
    0x0, // bDeviceProtocol
    64U, // bMaxPacketSize0      
    0x32,0x15, // idVendor
    0x43,0x00, // idProduct
    0x0,0x2, // bcdDevice
    0x1, // iManufacturer - no strings yet
    0x2, // iProduct - no strings yet
    0x3, // iSerialNumber - no strings yet
    0x1 // bNumConfigurations
};

// https://github.com/groupgets/LeptonModule/blob/master/software/STM32F3Discovery_ChibiOS/usbcfg.c - a modified version
uint8_t vcom_device_descriptor_data[DEV_DESC_SIZE] = 
{
    0x12, // bLength
    0x01, // bDescriptorType
    0x01,0x10,        /* bcdUSB (1.1).                    */
    0x02,          /* bDeviceClass (CDC).              */
    0x00,          /* bDeviceSubClass.                 */
    0x00,          /* bDeviceProtocol.                 */
    0x40,          /* bMaxPacketSize.                  */
    0x83,0x04,        /* idVendor (ST).                   */
    0x40,0x57,        /* idProduct.                       */
    0x00,0x02,        /* bcdDevice.                       */
    0x01,             /* iManufacturer.                   */
    0x02,             /* iProduct.                        */
    0x03,             /* iSerialNumber.                   */
    0x01             /* bNumConfigurations.              */
};

uint8_t config_descriptor_set[CONF_DESC_SET_SIZE] = 
{
    // http://www.usbmadesimple.co.uk/ums_4.htm
    // https://www.usb.org/sites/default/files/documents/hid1_11.pdf
    // configuration descriptor
    0x9, // bLength
    0x2, // bDescriptorType
    CONF_DESC_SET_SIZE, 0x0, // wTotalLength
    0x1, // bNumInterfaces
    0x1, // bConfigurationValue
    0x0, // no string yet// iConfiguration
    0x80, // bmAttributes
    100U, // bMaxPower - 200 mA
    
    // interface descriptor
    0x9, // bLength
    0x4, // bDescriptorType
    0x1, // bInterfaceNumber
    0x0, // bAlternateSetting
    0x1, // bNumEndpoints
    0x3, // bInterfaceClass - HID - needs a class specific descriptor TODO
    0x1, // bInterfaceSubClass
    0x2, // bInterfaceProtocol - mouse
    0x0, // iInterface
    
    // HID class descriptor - from hid1_11.pdf, pg 22
    0x9, // bLength
    0x21, // bDescriptorType - HID
    0x10, 0x1, // bcdHID
    0x0, // bCOuntryCode
    0x1, // bNumDescriptors
    0x22, // bDescriptorType - report descriptor
    0x32, 0x0, // wItemLength    
    
    // endpoint descriptor
    0x7, // bLength
    0x5, // bDescriptorType
    0x81, // bEndpointAddress
    0x3, // bmAttributes - interrupt endpoint - ideal for a mouse
    0x8, 0x0, // wMaxPacketSize - 8 bytes is plenty
    0xA // bInterval - 10 ms
};

/* string descriptors https://www.beyondlogic.org/usbnutshell/usb5.shtml#StringDescriptors */
uint8_t lang_descriptor[LANG_DESC_SIZE] = 
{
    0x4, // bLength
    0x3, // bDescriptorType
    0x9, 0x4 // wLANGID - English US
};

uint8_t vendor_string_descriptor[VENDOR_DESC_SIZE] = 
{
    14U, // bLength
    0x3, // bDescriptorType
    'V', 0x0, 'e', 0x0, 'n', 0x0, 'd', 0x0, 'o', 0x0, 'r',0x0
};

uint8_t product_string_descriptor[PRODUCT_DESC_SIZE] = 
{
    16U, // bLength
    0x3, // bDescriptorType
    'P', 0x0, 'r', 0x0, 'o', 0x0, 'd', 0x0, 'u', 0x0, 'c', 0x0, 't', 0x0
};

uint8_t sn_string_descriptor[SN_DESC_SIZE] =  // serial number
{
    10U, // bLength
    0x3, // bDescriptorType
    '1', 0x0, '3', 0x0, '3', 0x0, '7', 0x0
};

uint8_t qualifier_descriptor[QUALIFIER_DESC_SIZE] = // https://www.keil.com/pack/doc/mw/USB/html/_u_s_b__device__qualifier__descriptor.html
{
    10U, // bLength
    0x6, // bDescriptorType
    0x0, 0x2, // bcdUSB
    0x0, // bDeviceClass
    0x0, // bDeviceSubClass
    0x0, // bDeviceProtocol
    64U, // bMaxPacketSize0
    0x1, // bNumConfigurations
    0x0 // bReserved
};

#endif