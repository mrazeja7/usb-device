/* HW and USB init functions */
#ifndef __INITS_H
#define __INITS_H

void sysclk_init();
void usb_init();
void usb_core_init(); 
void nvic_init();
void enableTimerInterrupt();
void initializeTimer();
void initializeButtons();
#endif