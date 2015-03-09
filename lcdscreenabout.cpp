// Filename:    lcdscreenabout
// Description: shows program information screen
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include "lcdscreenabout.h"
#include "bmp_raspi.inc"

static lcdscreenabout aboutscreen;

lcdscreenabout::lcdscreenabout()
  : lcdscreen(NULL)
  , m_seconds(0)
  , m_contrast(9)
{
  setupScreen(2,this);
}

lcdscreenabout::~lcdscreenabout()
{
}

void lcdscreenabout::paintEvent()
{
  LCD_DrawBitmap(0,0,bmp_raspi);
  LCD_SetFont(0);
  LCD_PrintXY(66,4 ,"Raspi-HiFi");
  LCD_PrintXY(75,14,"Project");
  LCD_PrintXY(68,32,"powered by");
  LCD_PrintXY(68,42," Valentin");
  LCD_PrintXY(68,52,"  Illich");

  m_seconds = 0;
}

keyType lcdscreenabout::secTimer(struct tm */*result*/)
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
    lcdscreen::activateScreen(1);
    break;
  case eKeyB:
    //lcdscreen::activateScreen(2);
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
