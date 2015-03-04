// Filename:    raspilcd.h
// Description: Hardware abstraction layer for Raspi-LCD
//    
// Open Source Licensing GPL 3
//
// Author:      Martin Steppuhn, www.emsystech.de
//--------------------------------------------------------------------------------------------------

#ifndef RASPILCD_H
#define RASPILCD_H

//=== Includes =====================================================================================	

#include "std_c.h"
#include "bcm2835.h"

//=== Preprocessing directives (#define) ===========================================================

// Pin Setup for RaspiLCD

#define		PIN_LCD_RST			25
#define		PIN_LCD_CS			8
#define		PIN_LCD_RS			7
#define		PIN_LCD_MOSI		10
#define		PIN_LCD_SCLK		11
#define		PIN_LCD_BACKLIGHT	18

#define	LCD_RST_CLR 		bcm2835_gpio_clr(PIN_LCD_RST) 
#define	LCD_RST_SET			bcm2835_gpio_set(PIN_LCD_RST) 

#define	LCD_CS_CLR 			bcm2835_gpio_clr(PIN_LCD_CS)
#define	LCD_CS_SET			bcm2835_gpio_set(PIN_LCD_CS)

#define	LCD_RS_CLR 			bcm2835_gpio_clr(PIN_LCD_RS)
#define	LCD_RS_SET			bcm2835_gpio_set(PIN_LCD_RS)

#define	LCD_SPI_PUTC(__d)	SpiPutc(__d)
#define	LCD_SPI_WAIT_BUSY

#define		BUTTON_UP			(Button & 0x20)
#define		BUTTON_DOWN			(Button & 0x10)

#define		BUTTON_A			(Button & 0x08)
#define		BUTTON_B			(Button & 0x04)
#define		BUTTON_C			(Button & 0x02)
#define		BUTTON_D			(Button & 0x01)


#define		BUTTON_PRESSED_UP		(ButtonPressed & 0x20)
#define		BUTTON_PRESSED_DOWN		(ButtonPressed & 0x10)

#define		BUTTON_PRESSED_A		(ButtonPressed & 0x08)
#define		BUTTON_PRESSED_B		(ButtonPressed & 0x04)
#define		BUTTON_PRESSED_C		(ButtonPressed & 0x02)
#define		BUTTON_PRESSED_D		(ButtonPressed & 0x01)

//=== Type definitions (typedef) ===================================================================

//=== Global constants (extern) ====================================================================

//=== Global variables (extern) ====================================================================

extern uint8	Button;
extern uint8	ButtonPressed;

//=== Global function prototypes ===================================================================

int RaspiLcdHwInit(void);
void UpdateButtons(void);
int GetRaspberryHwRevision(void);
void SpiPutc(unsigned char d);
void SetBacklight(uint8 light);
void SleepMs(uint32 ms);

#endif
