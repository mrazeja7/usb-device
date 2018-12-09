#include "stm32f4xx_conf.h"
#include "delay.h"
void Delay(__IO uint32_t nTime)
{ 
    uwTimingDelay = nTime;

    while(uwTimingDelay-- != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
    if (uwTimingDelay != 0x00)
    { 
        uwTimingDelay--;
    }
}

void busyDelay()
{   
    for (int i=0; i<150; i++)
        for (int j=0; j<30000; j++);
}