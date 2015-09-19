// Filename:    main.cpp
// Description: main program for Raspi Hifi machine
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "lcdscreenmain.h"

#ifdef  QT_EMULATION
#include "QtEmulation.h"
#endif

typedef void (*sighandler_t)(int);
extern "C" sighandler_t signal(int signum, sighandler_t handler);

volatile bool running = true;

void terminateHandler()
{
  running = false;
}

int main(int argc, char **argv)
{
#ifdef  QT_EMULATION
  QApplication app(argc,argv);
#else
    signal(SIGTERM, (sighandler_t)terminateHandler);
    // avoid zombie processes...
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
    {
      perror(0);
      exit(1);
    }
#endif

  if( argc>1 )
  {
    if( strcmp(argv[1],"-debug")==0 )
      lcdscreen::setDebuMode(true);
  }

  myprintf("Raspi Hifi Recorder V1.0.0 by Valentin Illich [" __DATE__ " " __TIME__"]\n");
  myprintf("RaspberryHwRevision=%i\r\n",GetRaspberryHwRevision());

  if(!RaspiLcdHwInit()) { myprintf("RaspiLcdHwInit() failed!\r\n"); return 1; }
  LCD_Init();			// Init Display
  SetBacklight(1);	// Turn Backlight on

  //	DemoView = 0;
  //	DemoBubblesInit();
  lcdscreen::activateScreen(DEFAULT_SCREEN_ID);

#ifdef QT_EMULATION
  QtEmulation_Init();
  app.exec();
  QtEmulation_Exit();
  running = false;
#endif

#ifndef QT_EMULATION
  bcm2835_gpio_fsel(PIN_SWITCHER_OUT,BCM2835_GPIO_FSEL_OUTP);	// GPIO18 Output: Backlight
#endif

  while(running)
  {
    SleepMs(100); // Raspi B Rev. 2: this is about 16-17 times a second with drawing, 19-20 without...
    UpdateButtons();

    lcdscreen::updateDisplay();
    lcdscreen::updateTimer();

    if(BUTTON_PRESSED_UP)
      lcdscreen::keyPressed(eKeyUp);

    if(BUTTON_PRESSED_DOWN)
      lcdscreen::keyPressed(eKeyDown);

    if(BUTTON_PRESSED_A )
      lcdscreen::keyPressed(eKeyA);

    if(BUTTON_PRESSED_B )
      lcdscreen::keyPressed(eKeyB);

    if(BUTTON_PRESSED_C)
      lcdscreen::keyPressed(eKeyC);

    if(BUTTON_PRESSED_D)
      lcdscreen::keyPressed(eKeyD);

    //    LCD_WriteFramebuffer();
  }

  LCD_Init();			// Init Display
  SetBacklight(0);	// Turn Backlight off
  return(0);
}
