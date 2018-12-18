/* HW and USB init functions */
#ifndef __INITS_H
#define __INITS_H

void sysclkInit();
void usbInit();
void usbCoreInit(); 
void nvicInit();
void enableTimerInterrupt();
void initializeTimer();
void initializeButtons();
#endif