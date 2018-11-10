#include "stm32f4xx_conf.h"
#include "lcd.h"
#include "lcd_func.h"
#include <stdlib.h>
#include <stdio.h>
#define ROW_HEIGHT 24
#define CHARS_PER_LINE 20
#define MAX_LINES (240/ROW_HEIGHT)

void displayAsHex(uint8_t *data, uint16_t len)
{
    char *res = (char*) malloc(2*len*sizeof(char));
    char *pos = res;
    for (int i = 0; i < len; i++)
    {
        sprintf(pos, "%02X", data[i]);
        pos += 2;
    }
    displayText((uint8_t*)res, len*2, 1);
    free(res);
}

uint8_t nextRow = 0;
// vypise string reprezentovany polem bajtu
void displayText(uint8_t *data, uint16_t len, uint8_t overwrite)
{    
    uint8_t rows = len/CHARS_PER_LINE + 1;
    // overwrite = zacit psat na zacatek displeje?
    uint8_t startingRow = overwrite ? 0 : nextRow;
    if (startingRow >= MAX_LINES)
    {
        startingRow = 0;
        LCD_Clear(0xffff);
    }
    
    for (uint8_t line = 0; line < rows; ++line)
    {
        uint8_t j = 0;
        // zapisujeme po radcich
        for (uint8_t i = line*CHARS_PER_LINE; i < (line+1)*CHARS_PER_LINE; ++i)
        {
            if (i >= len)
            {
                // ulozime si cislo radku, na kterem budeme pripadne priste zacinat
                // pokud je zadany text dlouhy presne na jeden radek, neni nutne vkladat novy radek
                nextRow = startingRow + line + (len == CHARS_PER_LINE ? 0 : 1);
                break;
            }
            LCD_DisplayChar( (startingRow + line)*ROW_HEIGHT, 320 - ((j++)*16), data[i]);
        }
    }
    return;
}

// vypise promennou v dekadickem nebo hexadecimalnim tvaru
void displayNumber(uint16_t *num, uint8_t hex)
{
    char str[8];
    uint8_t len = sprintf(str, (hex ? "%02X" : "%u"), *num);
    displayText((uint8_t*) str, len, 1);
}