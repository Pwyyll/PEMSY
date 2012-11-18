#include <avr/io.h>
#include "pwm.h"

void lcd_init(void)
{
	//lcd_set_contrast(); // insert value!
	return;
}

void lcd_set_contrast(uint8_t contrast)
{
	set_ocr(contrast);
	return;
}

