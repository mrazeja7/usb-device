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
#define HID_REPORT_DESC_SIZE 50U

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

uint8_t mouse_descriptor[DEV_DESC_SIZE] = // Logitech mouse
{
    18U, // bLength
    0x1, // bDescriptorType
//    0x01,0x10, // bcdUSB
    0x0,0x2, // USB 2.0
    0x3, // bDeviceClass
    0x0, // bDeviceSubClass
    0x0, // bDeviceProtocol
    64U, // bMaxPacketSize0      
    0x6D,0x04, // idVendor
    0x2F,0xC5, // idProduct
    0x0,0x2, // bcdDevice
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
    0x1, // iManufacturer string
    0x2, // iProduct string
    0x3, // iSerialNumber string
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

uint8_t config_descriptor_set2[CONF_DESC_SET_SIZE] = 
{
    //////////		Standard Configuration Descriptor
		0x09, 	 	//	0	bLength	1	Number	Size of Descriptor in Bytes
		0x02,  		//	1	bDescriptorType	1	Constant	Configuration Descriptor (0x02)
		CONF_DESC_SET_SIZE, 0x0,				//	2	wTotalLength	2	Number	Total length in bytes of data returned
		0x01, 		//	4	bNumInterfaces	1	Number	Number of Interfaces //one interface
		0x01, 		//	5	bConfigurationValue	1	Number	Value to use as an argument to select this configuration
		0x00, 		//	6	iConfiguration	1	Index	Index of String Descriptor describing this configuration
		0x80, 		//	7	bmAttributes	1	Bitmap	D7 Reserved, set to 1. (USB 1.0 Bus Powered) D6 Self Powered, D5 Remote Wakeup, D4..0 Reserved, set to 0.
		0x32,		  //	8	bMaxPower	1	mA	Maximum Power Consumption in 2mA units//100 mA

		//////////		Standard Interface Descriptor
		0x09,			//	0	bLength	1	Number	Size of Descriptor in Bytes (9 Bytes)
		0x04,			//	1	bDescriptorType	1	Constant	Interface Descriptor (0x04)
		0x00,			//	2	bInterfaceNumber	1	Number	Number of Interface //should be 1 maybe??
		0x00,			//	3	bAlternateSetting	1	Number	Value used to select alternative setting
		0x01,			//	4	bNumEndpoints	1	Number	Number of Endpoints used for this interface
		0x03,			//	5	bInterfaceClass	1	Class	Class Code (Assigned by USB Org) 							//HID
		0x01,			//	6	bInterfaceSubClass	1	SubClass	Subclass Code (Assigned by USB Org)
		0x02,			//	7	bInterfaceProtocol	1	Protocol	Protocol Code (Assigned by USB Org)  //mouse
		0x00,			//	8	iInterface	1	Index	Index of String Descriptor Describing this interface
	
		//////////		Class Specific HID Descriptor
		0x09,     //  bLength: HID Descriptor size
		0x21,			//	bDescriptorType: HID
		0x10,0x01,//  bcdHID: Numeric expression identifying the HID Class Specification release (2 bytes)		
		0x00,     //	bCountryCode: Hardware target country
		0x01,     //  bNumDescriptors: Number of HID class descriptors to follow
		0x22,     //	bDescriptorType
		0x32,0x00,				//	wItemLength: Total length of Report descriptor!!!!!!!!!!!!
		
		//////////		Standard Endpoint Descriptor	
		0x07,     //	bLength: Endpoint Descriptor size
		0x05, 		//	bDescriptorType
		0x81,    	//	bEndpointAddress: Endpoint Address (IN) EP1/2???
		0x03,     //	bmAttributes: Interrupt endpoint
		0x08,0x00,//	wMaxPacketSize: 8 Byte max 
		0x0A      //	bInterval: Polling Interval (10 ms - 0x0A),(0xFF = 250 ms)
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
    'V', 0x0, 'e', 0x0, 'n', 0x0, 'd', 0x0, 'o', 0x0, 'r', 0x0
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

uint8_t mouse_report_descriptor[HID_REPORT_DESC_SIZE] = // hid1_11.pdf appendix E 10
{
    0x05,0x01,	//	Usage Page (Generic Desktop),		
    0x09,0x02,	//	Usage (Mouse),   		
    0xA1,0x01,	//	Collection (Application),		
    0x09,0x01,	//		Usage (Pointer),	
    0xA1,0x00,	//		Collection (Physical),	
    0x05,0x09,	//			Usage Page (Buttons),
    0x19,0x01,	//			Usage Minimum (01),
    0x29,0x03,	//			Usage Maximun (03),
    0x15,0x00,	//			Logical Minimum (0),
    0x25,0x01,	//			Logical Maximum (1),
    0x95,0x03,	//			Report Count (3),
    0x75,0x01,	//			Report Size (1),
    0x81,0x02,	//			Input (Data, Variable, Absolute), ;3 button bits   
    0x95,0x01,	//			Report Count (1),
    0x75,0x05,	//			Report Size (5),
    0x81,0x01,	//			Input (Constant), ;5 bit padding   
    0x05,0x01,	//			Usage Page (Generic Desktop),
    0x09,0x30,	//			Usage (X),
    0x09,0x31,	//			Usage (Y),
    0x15,0x81,	//			Logical Minimum (-127),
    0x25,0x7F,	//			Logical Maximum (127),
    0x75,0x08,	//			Report Size (8),
    0x95,0x02,	//			Report Count (2),
    0x81,0x06,	//			Input (Data, Variable, Relative), ;2 position bytes (X & Y)  
    0xC0,		//		End Collection,	
    0xC0		//	End Collection	
};

#endif