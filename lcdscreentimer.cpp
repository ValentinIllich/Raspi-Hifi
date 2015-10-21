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
#include "lcdscreenmessages.h"
#include "screenids.h"

static char timerText1[128];
static char timerText2[128];
static char timerText3[128];
static char timerText4[128];
static char timerText5[128];

static char *timerText[] = { timerText1,timerText2,timerText3,timerText4,timerText5 };

static objectinfo strings[] = {
  { eText,true,   0, 0,  0, 0,  0, "" },
  { eText,true,   0,10,  0, 0,  0, "" },
  { eText,true,   0,20,  0, 0,  0, "" },
  { eText,true,   0,30,  0, 0,  0, "" },
  { eText,true,   0,40,  0, 0,  0, "" },

  { eText,true,   0,56,  0, 0,  0," Back" },
  { eText,true,  50,56,  0, 0,  0,"" }, // Manu / Auto Display
  { eText,true, 100,56,  0, 0,  0,"" }, // Set / On/Off Display
  { eNone,false, 0,0,0,0,  0,NULL },
};

bool lcdscreentimer::m_timerActive = false;

static lcdscreentimer timerscreen;

lcdscreentimer::lcdscreentimer()
  : lcdscreen(strings)
  , m_updateTimerSettings(false)
  , m_timercount(0)
  , m_lastSwitcherAutoState(true)
  , m_lastSwitcherManuState(false)
  , m_lastRecordState(false)
  , m_selectedTime(0)
  , m_editMode(false)
  , m_timerSeconds(-1)
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
  {
    if( m_timerSeconds>=0 )
      strings[6].text = "Wait";
    else
      strings[6].text = "Stop";

    if( m_lastRecordState )
      strings[7].text = "Rec";
    else if( m_lastSwitcherManuState )
      strings[7].text = "Off";
    else
      strings[7].text = "On";
  }
  else
  {
    strings[6].text = "Start";
    strings[7].text = "Set";
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

  if( m_timerSeconds>=0 )
  {
    m_timerSeconds++;

    if( m_timerSeconds>10 )
    {
      switchPower(!m_timerActive, m_lastSwitcherManuState);
      m_timerSeconds = -1;
      activatedHandler();
    }
  }

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

    if( m_timerSeconds==-1 )
      switchPower(switcherOn, m_lastSwitcherManuState);

    if( recordOn!=m_lastRecordState )
    {
      m_lastRecordState = recordOn;
      myprintf("record!\n");
      lcdscreenmain *main = static_cast<lcdscreenmain*>(lcdscreen::getScreen(RECORD_PLAY_SCREEN));
      if( recordOn )
        main->startRecording();
      else
        main->stopRecording();

      activatedHandler();
    }
  }

  return eKeyNone;
}

void lcdscreentimer::switchPower( bool switcherAutoState, bool switcherManuState )
{
  bool update = false;

  if( switcherAutoState!=m_lastSwitcherAutoState )
  {
    m_lastSwitcherAutoState = switcherAutoState;
    update = true;
  }
  if( switcherManuState!=m_lastSwitcherManuState )
  {
    m_lastSwitcherManuState = switcherManuState;
    update = true;
  }

  if( update )
  {
    bool state = m_lastSwitcherAutoState || m_lastSwitcherManuState;
    myprintf("switching %s!\n",state ? "On" : "Off");
    //QMessageBox::warning(0,"",state ? "On" : "Off");
#ifndef QT_EMULATION
    if( state )
    {
      bcm2835_gpio_clr(PIN_SWITCHER_OUT);
    }
    else
    {
      bcm2835_gpio_set(PIN_SWITCHER_OUT);
    }
#endif
  }
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
/*      if( m_timerActive && m_lastRecordState )
      {
        lcdscreenmain *main = static_cast<lcdscreenmain*>(lcdscreen::getScreen(RECORD_PLAY_SCREEN));
        main->stopRecording();
      }*/
      m_timerActive = !m_timerActive;
      if( m_timerActive )
        m_timerSeconds = 0; // start timer for auto switching
      else
        switchPower(!m_timerActive, false);
      activatedHandler();
    }
    repaint();
    break;
  case eKeyC:
    if( m_timerActive )
    {
      m_timerSeconds = -1; // reset timer for auto switching
      if( m_lastRecordState )
      {
        lcdscreenQuestion::setMessage("   Recording!");
        lcdscreenQuestion::setButtons("");
        lcdscreen::activateScreen(MESSAGE_SCREEN);
      }
      else if( m_lastSwitcherManuState )
      {
        switchPower(m_lastSwitcherAutoState, false);
//        strings[7].text = "On";
      }
      else
      {
        switchPower(m_lastSwitcherAutoState, true);
//        strings[7].text = "Off";
      }
      activatedHandler();
    }
    else
    {
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
        myprintf("+++ could not write timer file!\n");
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
