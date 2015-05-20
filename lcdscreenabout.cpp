// Filename:    lcdscreenabout
// Description: shows program information screen
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lcdscreenabout.h"
#include "lcdscreenedit.h"
#include "bmp_raspi.inc"

#include "screenids.h"

static objectinfo strings[] = {
  { eText,true,   0,56,  0, 0,  0,"Back" },
  { eText,true,  50,56,  0, 0,  0,"Clock    Info" },

  { eNone,false, 0,0,0,0,  0,NULL },
};

static lcdscreenabout aboutscreen;

lcdscreenabout::lcdscreenabout()
  : lcdscreen(strings)
  , m_seconds(0)
  , m_contrast(9)
{
  setupScreen(ABOUT_SCREEN,this);
}

lcdscreenabout::~lcdscreenabout()
{
}

void lcdscreenabout::paintEvent()
{
  LCD_DrawBitmap(0,0,bmp_raspi);
  LCD_SetFont(0);
  LCD_PrintXY(66,0 ,"Raspi-HiFi");
  LCD_PrintXY(75,10,"Project");
  LCD_PrintXY(68,20,"powered by");
  LCD_PrintXY(68,30," Valentin");
  LCD_PrintXY(68,40,"  Illich");

  m_seconds = 0;
}

keyType lcdscreenabout::secTimerHandler(struct tm */*result*/)
{
  m_seconds++;

  if( m_seconds>10 )
  {
    return eKeyCancel;
  }

  return eKeyNone;
}

keyType lcdscreenabout::keyEventHandler( keyType key )
{
  char currentTime[64];
  switch( key )
  {
  case eKeyA:
    lcdscreen::activateScreen(RECORD_PLAY_SCREEN);
    break;
  case eKeyB:
    {
      time_t clock = time(NULL);
      struct tm start = *localtime(&clock);
      sprintf(currentTime,"%02d.%02d.%02d  %02d:%02d:%02d",
              start.tm_mday,start.tm_mon+1,start.tm_year-100,start.tm_hour,start.tm_min,start.tm_sec);
      lcdscreenedit::setInputString(currentTime);
      lcdscreen::activateScreen(EDIT_SCREEN);
      //lcdscreen::activateScreen(2);
    }
    break;
  case eKeyCancel:
    if( strlen(lcdscreenedit::getInputString())>0 )
    {
      struct tm start, stop;
      if( lcdscreenedit::scantimerSetting(lcdscreenedit::getInputString(),editFotmatSettingTime,start,stop) )
      {
        // set system time / real time clock <hwclock --set --date="2015-05-03 19:28:00">
        sprintf(currentTime,"sudo hwclock --set --date=\"%04d-%02d-%02d %02d:%02d:%02d\"",
                start.tm_year+1900,start.tm_mon+1,start.tm_mday,start.tm_hour,start.tm_min,start.tm_sec
                );
        system(currentTime);
        system("sudo hwclock -s");
        return eKeyCancel;
      }
    }
    break;
  case eKeyC:
    lcdscreen::activateScreen(CPUTEMP_SCREEN);
    break;
  case eKeyUp:
    if( m_contrast < 20 ) m_contrast++;
    LCD_SetContrast(m_contrast);
    break;
  case eKeyDown:
    if( m_contrast > 0 ) m_contrast--;
    LCD_SetContrast(m_contrast);
    break;
  default:
    break;
  }

  return eKeyNone;
}
