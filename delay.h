#ifndef DELAY_H
#define DELAY_H
static __IO uint32_t uwTimingDelay;
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
void busyDelay();
#endif