#ifndef LCD_FUNC_H
#define LCD_FUNC_H
void displayAsHex(uint8_t *data, uint16_t len);
void displayText(uint8_t *data, uint16_t len, uint8_t overwrite);
void displayNumber(uint16_t *num, uint8_t hex);
#endif