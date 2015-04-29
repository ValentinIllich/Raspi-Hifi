// Filename:    lcdscreentimer
// Description: handling of up to 5 timers: setting times / doing switching / starting, stopping records
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lcdscreentimer.h"
#include "lcdscreenmain.h"
#include "lcdscreenedit.h"
#include "screenids.h"

static char timerText1[128];
static char timerText2[128];
static char timerText3[128];
static char timerText4[128];
static char timerText5[128];

static char timerTextEdit[128];

static char *timerText[] = { timerText1,timerText2,timerText3,timerText4,timerText5 };

static objectinfo strings[] = {
  { eText,true,   0, 0,  0, 0,  0, "" },
  { eText,true,   0,10,  0, 0,  0, "" },
  { eText,true,   0,20,  0, 0,  0, "" },
  { eText,true,   0,30,  0, 0,  0, "" },
  { eText,true,   0,40,  0, 0,  0, "" },

  { eText,true,   0,56,  0, 0,  0," Back            Set" },
  { eText,true,  54,56,  0, 0,  0,"" },
  { eNone,false, 0,0,0,0,  0,NULL },
};

bool lcdscreentimer::m_timerActive = false;

static lcdscreentimer timerscreen;

lcdscreentimer::lcdscreentimer()
  : lcdscreen(strings)
  , m_updateTimerSettings(false)
  , m_timercount(0)
  , m_lastSwitcherState(false)
  , m_lastRecordState(false)
  , m_selectedTime(0)
  , m_editMode(false)
{
  lcdscreen::setupScreen(TIMER_SCREEN,&timerscreen);
  m_updateTimerSettings = true;
}

lcdscreentimer::~lcdscreentimer()
{
}

bool lcdscreentimer::timerActive()
{
  return m_timerActive;
}

void lcdscreentimer::activatedHandler()
{
  if( m_timerActive )
    strings[6].text = "Auto";
  else
  {
    strings[6].text = "Manu";
    strings[6].visible = true;
  }

  for( int i=0; i<5; i++ )
  {
    if( m_timerStart[i]==-1 || m_timerStop[i]==-1 )
      strcpy(timerText[i],"");
    else
    {
      struct tm start;
      start = *lcdscreen::fromTimeInSeconds(m_timerStart[i]);
      struct tm stop;
      stop = *lcdscreen::fromTimeInSeconds(m_timerStop[i]);

      sprintf(timerText[i],"%02d.%02d.%02d %02d:%02d-%02d:%02d",
              start.tm_mday,start.tm_mon+1,start.tm_year-100,start.tm_hour,start.tm_min,
              stop.tm_hour,stop.tm_min);
    }
  }
}

keyType lcdscreentimer::secTimerHandler(struct tm *result)
{
  int actualTimerSecs = lcdscreen::toTimeInSeconds(result);

  if( m_timerActive )
  {
    if( actualTimerSecs%2 )
      strings[6].visible = true;
    else
      strings[6].visible = false;

    repaint();
  }

  if( m_updateTimerSettings )
  {
    m_timercount = 0;
    for( int i=0; i<5; i++ )
      m_timerStart[i] = m_timerStop[i] = -1;

    FILE *fp = fopen("/home/pi/usbstick/timer.txt","r");
    if( fp )
    {
     char buffer[1024];

      while( fgets(buffer,1023,fp) )
        scantimerSetting(buffer);

      fclose(fp);
    }

    m_updateTimerSettings = false;
  }

  if( m_timerActive )
  {
    bool switcherOn = false;
    bool recordOn = false;

    for( int i=0; i<5; i++ )
    {
      if( m_timerStart[i]==-1 || m_timerStop[i]==-1 )
        continue;

      if( actualTimerSecs>=(m_timerStart[i]-60) && actualTimerSecs<=(m_timerStop[i]+60) )
        switcherOn = true;
      if( actualTimerSecs>=m_timerStart[i] && actualTimerSecs<=m_timerStop[i] )
        recordOn = true;
    }

    if( switcherOn!=m_lastSwitcherState )
    {
      m_lastSwitcherState = switcherOn;
      fprintf(stderr,"switching!\n");
#ifndef QT_EMULATION
      if( switcherOn )
        bcm2835_gpio_clr(PIN_SWITCHER_OUT);
      else
        bcm2835_gpio_set(PIN_SWITCHER_OUT);
#endif
    }
    if( recordOn!=m_lastRecordState )
    {
      m_lastRecordState = recordOn;
      fprintf(stderr,"record!\n");
      lcdscreenmain *main = static_cast<lcdscreenmain*>(lcdscreen::getScreen(RECORD_PLAY_SCREEN));
      if( recordOn )
        main->startRecording();
      else
        main->stopRecording();
    }
  }

  return eKeyNone;
}

keyType lcdscreentimer::keyEventHandler( keyType key )
{
  switch( key )
  {
  case eKeyA:
    if( m_editMode )
    {
    }
    else
      return eKeyCancel;
    break;
  case eKeyB:
    if( m_editMode )
    {
    }
    else
    {
      m_timerActive = !m_timerActive;
  #ifndef QT_EMULATION
      if( m_timerActive )
        bcm2835_gpio_set(PIN_SWITCHER_OUT);
      else
        bcm2835_gpio_clr(PIN_SWITCHER_OUT);
  #endif
      activatedHandler();
    }
    repaint();
    break;
  case eKeyC:
    m_editMode = !m_editMode;
    if( m_editMode )
    {
      if( m_selectedTime>=0 )
      {
        if( m_timerStart[m_selectedTime]==-1 )
        {
          lcdscreenedit::setInputString("");
        }
        else
          lcdscreenedit::setInputString(timerText[m_selectedTime]);
      }
//      else
//        lcdscreenedit::setInputString("");
      lcdscreen::activateScreen(EDIT_SCREEN);
    }
    repaint();
    break;
  case eKeyCancel:
    if( strlen(lcdscreenedit::getInputString())>0 )
    {
      if( !scantimerSetting(lcdscreenedit::getInputString(),m_selectedTime) )
//        m_timercount = m_selectedTime+1;
//      else
        m_timerStart[m_selectedTime] = m_timerStop[m_selectedTime] = -1;

      FILE *fp = fopen("/home/pi/usbstick/timer.txt","w");
      if( fp )
      {
        for( int i=0; i<5; i++ )
        {
          if( m_timerStart[i]>=0 && m_timerStop[i]>=0 )
          {
            struct tm starting = *lcdscreen::fromTimeInSeconds(m_timerStart[i]);
            struct tm stopping = *lcdscreen::fromTimeInSeconds(m_timerStop[i]);

            fprintf(fp,"%02d.%02d.%04d %02d:%02d-%02d:%02d\n",starting.tm_mday,starting.tm_mon+1,starting.tm_year+1900,
                    starting.tm_hour,starting.tm_min,stopping.tm_hour,stopping.tm_min);
          }
        }
        fclose(fp);
      }
      else
        fprintf(stderr,"+++ could not write timer file!\n");
    }
    m_editMode = false;
    activatedHandler();
    break;
  case eKeyUp:
    if( m_selectedTime>=0 ) m_selectedTime--;
    repaint();
    break;
  case eKeyDown:
    if( m_selectedTime<4 ) m_selectedTime++;
    repaint();
    break;
  default:
    break;
  }

  return eKeyNone;
}

/*char *lcdscreentimer::substring( char *str, int start, int len )
{
}

void lcdscreentimer::incdecChar( char *str, int pos, int delta )
{
}*/

void lcdscreentimer::paintEvent()
{
  if( m_editMode )
  {
  }
  else
  {
    if( m_selectedTime>=0 )
    {
      int x = 0, y = 10*m_selectedTime, dx = 127, dy = 10;
      LCD_SetPenColor(1);
      LCD_DrawLine(x,y,x+dx,y);
      LCD_DrawLine(x+dx,y,x+dx,y+dy);
      LCD_DrawLine(x+dx,y+dy,x,y+dy);
      //LCD_DrawLine(x,y+dy,x,y); // does not really work ?
      LCD_DrawLine(x,y,x,y+dy);
    }
    LCD_SetFont(0);
    LCD_PrintXY(3, 2,timerText1);
    LCD_PrintXY(3,12,timerText2);
    LCD_PrintXY(3,22,timerText3);
    LCD_PrintXY(3,32,timerText4);
    LCD_PrintXY(3,42,timerText5);
  }
}

bool lcdscreentimer::scantimerSetting( const char *source, int editedTime )
{
  bool ret = true;

  struct tm start;
  struct tm stop;

  if( editedTime>=0 )
    ret = lcdscreenedit::scantimerSetting(source,editFormatStartStopShort,start,stop);
  else
    ret = lcdscreenedit::scantimerSetting(source,editFormatStartStopLong,start,stop);

  if( ret )
  {
    if( editedTime>=0 )
    {
      m_timerStart[editedTime] = lcdscreen::toTimeInSeconds(&start);
      m_timerStop[editedTime] = lcdscreen::toTimeInSeconds(&stop);

      if( m_timerStop[editedTime] < m_timerStart[editedTime] )
        m_timerStop[editedTime] += 86400;
    }
    else
    {
      m_timerStart[m_timercount] = lcdscreen::toTimeInSeconds(&start);
      m_timerStop[m_timercount] = lcdscreen::toTimeInSeconds(&stop);

      if( m_timerStop[m_timercount] < m_timerStart[m_timercount] )
        m_timerStop[m_timercount] += 86400;

      m_timercount++;

      if( m_timercount==5 )
        return true;
    }
  }

  return ret;
}