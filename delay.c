#include "stm32f4xx_conf.h"
#include "delay.h"
/* code lifted from the LCD example. It's bad but it works. */

void busyDelay()
{   
    for (int i=0; i<150; i++)
        for (int j=0; j<30000; j++);
}

void shortDelay()
{   
    for (int i=0; i<50; i++)
        for (int j=0; j<30000; j++);
}