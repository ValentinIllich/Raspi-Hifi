// Filename:    lcdscreenabout
// Description: shows program information screen
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include "lcdscreenabout.h"
#include "bmp_raspi.inc"

#include "screenids.h"

static objectinfo strings[] = {
  { eText,true,  54,56,  0, 0,  0,"Help    Info" },

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
  switch( key )
  {
  case eKeyA:
    lcdscreen::activateScreen(RECORD_PLAY_SCREEN);
    break;
  case eKeyB:
    //lcdscreen::activateScreen(2);
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
