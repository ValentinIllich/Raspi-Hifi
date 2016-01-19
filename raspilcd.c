// Filename:    raspilcd.c
// Description: Hardware abstraction layer for Raspi-LCD
//
// Open Source Licensing GPL 3
//
// Author:      Martin Steppuhn, www.emsystech.de
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

//=== Includes =====================================================================================

#include "std_c.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raspilcd.h"

#ifdef QT_EMULATION
#include "QtEmulation.h"
#endif

//=== Preprocessing directives (#define) ===========================================================

//=== Type definitions (typedef) ===================================================================

//=== Global constants =============================================================================

//=== Global variables =============================================================================

uint8	Button,ButtonPressed;

//=== Local constants  =============================================================================

//=== Local variables ==============================================================================

uint8	PinButton[6];
uint8	ButtonMem;

//=== Local function prototypes ====================================================================


//--------------------------------------------------------------------------------------------------
// Name:      HalInit
// Function:  Setup GPIO for Raspi_LCD
//            
// Parameter: -
// Return:    -
//--------------------------------------------------------------------------------------------------
int RaspiLcdHwInit(void)
{
#ifdef QT_EMULATION
  return 1; // QT-Simul!
#endif
  int HwRev;

  HwRev = GetRaspberryHwRevision();

  if (!bcm2835_init()) return 0;

  // Buttons
  PinButton[0] = 17;
  PinButton[1] = 4;
  PinButton[2] = 24;
  PinButton[3] = 23;
  PinButton[4] = 22;
  PinButton[5] = (HwRev < 2) ? 21 : 27;;

  bcm2835_gpio_fsel(PinButton[0],BCM2835_GPIO_FSEL_INPT)	;	// Set GPIO Pin to Input
  bcm2835_gpio_fsel(PinButton[1],BCM2835_GPIO_FSEL_INPT)	;	// Set GPIO Pin to Input
  bcm2835_gpio_fsel(PinButton[2],BCM2835_GPIO_FSEL_INPT)	;	// Set GPIO Pin to Input
  bcm2835_gpio_fsel(PinButton[3],BCM2835_GPIO_FSEL_INPT)	;	// Set GPIO Pin to Input
  bcm2835_gpio_fsel(PinButton[4],BCM2835_GPIO_FSEL_INPT)	;	// Set GPIO Pin to Input
  bcm2835_gpio_fsel(PinButton[5],BCM2835_GPIO_FSEL_INPT)	;	// Set GPIO Pin to Input

  bcm2835_gpio_set_pud(PinButton[0],BCM2835_GPIO_PUD_UP); 	// Enable Pullup
  bcm2835_gpio_set_pud(PinButton[1],BCM2835_GPIO_PUD_UP); 	// Enable Pullup
  bcm2835_gpio_set_pud(PinButton[2],BCM2835_GPIO_PUD_UP); 	// Enable Pullup
  bcm2835_gpio_set_pud(PinButton[3],BCM2835_GPIO_PUD_UP); 	// Enable Pullup
  bcm2835_gpio_set_pud(PinButton[4],BCM2835_GPIO_PUD_UP); 	// Enable Pullup
  bcm2835_gpio_set_pud(PinButton[5],BCM2835_GPIO_PUD_UP); 	// Enable Pullup

  // LCD Display
  bcm2835_gpio_fsel(PIN_LCD_MOSI,     BCM2835_GPIO_FSEL_OUTP);	// GPIO10 Output: MOSI
  bcm2835_gpio_fsel(PIN_LCD_SCLK,     BCM2835_GPIO_FSEL_OUTP);	// GPIO11 Output: SCLK
  bcm2835_gpio_fsel(PIN_LCD_RST,      BCM2835_GPIO_FSEL_OUTP);	// GPIO25 Output: RST
  bcm2835_gpio_fsel(PIN_LCD_CS ,      BCM2835_GPIO_FSEL_OUTP);	// GPIO8  Output: CS
  bcm2835_gpio_fsel(PIN_LCD_RS,       BCM2835_GPIO_FSEL_OUTP);	// GPIO7  Output: RS
  bcm2835_gpio_fsel(PIN_LCD_BACKLIGHT,BCM2835_GPIO_FSEL_OUTP);	// GPIO18 Output: Backlight

  Button = ButtonMem = ButtonPressed = 0;

  return 1;
}

//--------------------------------------------------------------------------------------------------
// Name:      UpdateButtons
// Function:  Read button states and save them to the variable "Button" and "ButtonPressed"
//            
// Parameter: 
// Return:    
//--------------------------------------------------------------------------------------------------
void UpdateButtons(void)
{
#ifdef QT_EMULATION
  return; // QT-Simul!
#endif
  ButtonMem = Button;		// Save last State

  Button = 0;
  if(!bcm2835_gpio_lev(PinButton[0])) Button |= (1<<0);
  if(!bcm2835_gpio_lev(PinButton[1])) Button |= (1<<1);
  if(!bcm2835_gpio_lev(PinButton[2])) Button |= (1<<2);
  if(!bcm2835_gpio_lev(PinButton[3])) Button |= (1<<3);
  if(!bcm2835_gpio_lev(PinButton[4])) Button |= (1<<4);
  if(!bcm2835_gpio_lev(PinButton[5])) Button |= (1<<5);

  ButtonPressed = (Button ^ ButtonMem) & Button;			// Set by Pressing a Button

  static int repeat = 0;

  if( Button!=0 )
  {
    repeat++;
    if( repeat>5 ) ButtonPressed = Button;
  }
  else
    repeat=0;
}

//--------------------------------------------------------------------------------------------------
// Name:        GetRaspberryHwRevision
// Function:  	Check wich Hardware is used:
//				http://www.raspberrypi.org/archives/1929
//	
//				Model B Revision 1.0 									2
//				Model B Revision 1.0 + ECN0001 (no fuses, D14 removed) 	3
//				Model B Revision 2.0 									4, 5, 6
//            
// Parameter: 	-
// Return:      0=no info , 1=HW Rev.1, 2=HW Rev.2
//--------------------------------------------------------------------------------------------------
int GetRaspberryHwRevision(void)
{
#ifdef QT_EMULATION
  return 0; // QT-Simul!
#endif
  FILE *fp;
  char line[32];
  char s[32];
  int i;

  fp = fopen("/proc/cpuinfo", "r");		// open as file
  if(fp != NULL)
  {
    while(fgets(line,32,fp))			// get line
    {
      sscanf(line,"%s : %x",(char*)&s,&i);		// parse for key and value
      if(strcmp(s,"Revision") == 0)				// check for "Revision"
      {
        //printf("Found: %s=%i\r\n",s,i);
        if(i < 4)  return 1;
        else		return 2;
      }
    }
  }
  else
  {
    //printf("cpuinfo not available.\r\n");
    return 0;
  }
  //printf("no revision info available.\r\n");
  return 0;
}


//--------------------------------------------------------------------------------------------------
// Name:    	SpiPutc
// Function:  	Emulate SPI on GPIO (Bitbanging)
//            
// Parameter: 	Databyte to send
// Return:      -
//--------------------------------------------------------------------------------------------------
void SpiPutc(unsigned char d)
{
#ifdef QT_EMULATION
  return; // QT-Simul!
#endif
  int i,n;

  for(i=0;i<8;i++)
  {
    if(d & 0x80)	bcm2835_gpio_set(PIN_LCD_MOSI);		// MOSI = 1
    else		bcm2835_gpio_clr(PIN_LCD_MOSI);		// MOSI = 0
    d <<= 1;

    for(n=0;n<4;n++) bcm2835_gpio_clr(PIN_LCD_SCLK); 	// CLK = 0
    for(n=0;n<4;n++) bcm2835_gpio_set(PIN_LCD_SCLK);	// CLK = 1
  }
}

//--------------------------------------------------------------------------------------------------
// Name:    	SetBacklight
// Function:  	Hintergrundbeleuchtung
//            
// Parameter: 	0=Off 1=On
// Return:      -
//--------------------------------------------------------------------------------------------------
void SetBacklight(uint8 light)
{
#ifdef QT_EMULATION
  Qt_SetBacklight(light);
  return; // QT-Simul!
#endif
  if(light)	bcm2835_gpio_set(PIN_LCD_BACKLIGHT);
  else	bcm2835_gpio_clr(PIN_LCD_BACKLIGHT)	;
}

//--------------------------------------------------------------------------------------------------
// Name:    	SleepMs
// Function:  	Sleep /Delay in Milliseconds
//            
// Parameter: 	Milliseconds
// Return:      -
//--------------------------------------------------------------------------------------------------
void SleepMs(uint32 ms)
{
#ifdef QT_EMULATION
  return; // QT-Simul!
#endif
  bcm2835_delay(ms);
}


