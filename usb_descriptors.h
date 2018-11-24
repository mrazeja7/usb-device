/* USB descriptor definitions */
#ifndef __USB_DESCS_H
#define __USB_DESCS_H

uint8_t device_descriptor[18] = // 18 bytes
{
    18U, // bLength
    0x1, // bDescriptorType
    0x01,0x10, // bcdUSB
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

// https://github.com/groupgets/LeptonModule/blob/master/software/STM32F3Discovery_ChibiOS/usbcfg.c - a modified version
uint8_t vcom_device_descriptor_data[18] = 
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

//uint8_t config_descriptor[] = 
//{
//    // bLength
//    // bDescriptorType
//    // bNumInterfaces
//    // bConfigurationValue
//    // bmAttributes
//    // bMaxPower
//};

#endif