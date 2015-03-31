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

#include "lcdscreentimer.h"
#include "lcdscreenmain.h"
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
  , m_selectedPos(-1)
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
      if( m_selectedPos>0 )
        m_selectedPos--;
      repaint();
    }
    else
      return eKeyCancel;
    break;
  case eKeyB:
    if( m_editMode )
    {
      if( m_selectedPos<19 )
        m_selectedPos++;
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
          m_timercount = m_selectedTime+1;
          strcpy(timerTextEdit,"26.03.15 10:05-10:07");
        }
        else
          strcpy(timerTextEdit,timerText[m_selectedTime]);
      }
      m_selectedPos = 0;
    }
    else
    {
      if( !scantimerSetting(timerTextEdit,m_selectedTime) )
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
      m_editMode = false;
      activatedHandler();
    }
    repaint();
    break;
  case eKeyUp:
    if( m_editMode )
      incdecChar(timerTextEdit,m_selectedPos,+1);
    else if( m_selectedTime>=0 ) m_selectedTime--;
    repaint();
    break;
  case eKeyDown:
    if( m_editMode )
      incdecChar(timerTextEdit,m_selectedPos,-1);
    else if( m_selectedTime<4 ) m_selectedTime++;
    repaint();
    break;
  default:
    break;
  }

  return eKeyNone;
}

char *lcdscreentimer::substring( char *str, int start, int len )
{
  strcpy( m_substring, str+start );
  m_substring[len] = 0x0;

  return m_substring;
}

void lcdscreentimer::incdecChar( char *str, int pos, int delta )
{
  char actual = str[pos];

  switch( actual )
  {
  case '0':
    if( delta>0 )
      actual += delta;
    else
      actual = '-';
    break;
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
    actual += delta;
    break;
  case '9':
    if( delta>0 )
      actual = '0';
    else
      actual += delta;
    break;
  case '-':
    if( delta>0 )
      actual = '0';
    break;
  case '.':
    break;
  }

  str[pos] = actual;
}

void lcdscreentimer::paintEvent()
{
  if( m_editMode )
  {
//    LCD_SetFont(0);
//    LCD_PrintXY(0, 0,timerText1);
    LCD_SetFont(1);
    LCD_PrintXY(30,10,substring(timerTextEdit,0,9));
    LCD_PrintXY(20,26,substring(timerTextEdit,9,11));

    int x = 0, y = 0, dx = 8, dy = 14;
    if( m_selectedPos<9 )
    {
      x = 30+m_selectedPos*8; y = 10;
    }
    else
    {
      x = 20+(m_selectedPos-9)*8, y = 26;
    }
    LCD_SetPenColor(1);
    LCD_DrawLine(x,y,x+dx,y);
    LCD_DrawLine(x+dx,y,x+dx,y+dy);
    LCD_DrawLine(x+dx,y+dy,x,y+dy);
    //LCD_DrawLine(x,y+dy,x,y); // does not really work ?
    LCD_DrawLine(x,y,x,y+dy);
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
  // "15.03.2015 10:05-10:07"
  char setting[128];

  strcpy(setting,source);

  if( setting[0]=='-' ) return false;

  if( setting[2]!='.' ) return false; else setting[2]=0x0;
  if( setting[5]!='.' ) return false; else setting[5]=0x0;

  if( editedTime>=0 )
  {
    if( setting[8]!=' ' ) return false; else setting[8]=0x0;
  }
  else
  {
    if( setting[10]!=' ' ) return false; else setting[10]=0x0;
  }

  if( editedTime>=0 )
  {
    if( setting[11]!=':' ) return false; else setting[11]=0x0;
  }
  else
  {
    if( setting[13]!=':' ) return false; else setting[13]=0x0;
  }

  if( editedTime>=0 )
  {
    if( setting[14]!='-' ) return false; else setting[14]=0x0;
  }
  else
  {
    if( setting[16]!='-' ) return false; else setting[16]=0x0;
  }

  if( editedTime>=0 )
  {
    if( setting[17]!=':' ) return false; else setting[17]=0x0;
  }
  else
  {
    if( setting[19]!=':' ) return false; else setting[19]=0x0;
  }

  struct tm start;
  start.tm_mday = atoi(setting);
  start.tm_mon = atoi(setting+3) - 1;
  if( editedTime>=0 )
  {
    start.tm_year = atoi(setting+6) + 100;
    start.tm_hour = atoi(setting+9);
    start.tm_min = atoi(setting+12);
  }
  else
  {
    start.tm_year = atoi(setting+6) - 1900;
    start.tm_hour = atoi(setting+11);
    start.tm_min = atoi(setting+14);
  }
  start.tm_sec = 0;

  struct tm stop = start;
  if( editedTime>=0 )
  {
    stop.tm_hour = atoi(setting+15);
    stop.tm_min = atoi(setting+18);
  }
  else
  {
    stop.tm_hour = atoi(setting+17);
    stop.tm_min = atoi(setting+20);
  }

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

  return true;
}
