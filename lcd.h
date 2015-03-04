// Filename:    lcd.h
// Description: Hardware abstraction layer for Raspi-LCD
//
// Open Source Licensing GPL 3
//
// Author:      Martin Steppuhn, www.emsystech.de
//--------------------------------------------------------------------------------------------------
#ifndef LCD_H
#define LCD_H

//=== Includes =====================================================================================	

#include "std_c.h"

//=== Preprocessing directives (#define) ===========================================================

#define		LCD_WIDTH		128
#define		LCD_HEIGHT		64
#define		LCD_X_OFFSET	4		// Pixel Offset

//=== Type definitions (typedef) ===================================================================

//=== Global constants (extern) ====================================================================

//=== Global variables (extern) ====================================================================

//=== Global function prototypes ===================================================================
#ifdef __cplusplus
extern "C"
{
#endif

void LCD_ClearScreen(void);
void LCD_SetPenColor(uint8 c);
void LCD_SetFillColor(int8 c);
void LCD_SetFont(uint8 f);
void LCD_SetContrast(uint8 contrast);

void LCD_PutPixel(uint8 x,uint8 y,uint8 color);
void LCD_DrawLine(uint8 x0,uint8 y0,uint8 x1,uint8 y1);
void LCD_DrawCircle(uint8 x0,uint8 y0,uint8 radius);
void LCD_DrawEllipse(int xm, int ym, int a, int b);
void LCD_DrawRect(uint8 x0,uint8 y0,uint8 x1,uint8 y1,uint8 line);
void LCD_PrintXY(uint8 x,uint8 y,const char *s);
void LCD_DrawBitmap(uint8 x0,uint8 y0,const uint8 *bmp);

void LCD_Init(void);
void LCD_WriteFramebuffer(void);

#ifdef __cplusplus
}
#endif

#endif
