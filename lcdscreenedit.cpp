#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lcdscreenedit.h"
#include "screenids.h"

static objectinfo strings[] = {
//  { eText,true,   0, 0,  0, 0,  0, "" },
//  { eText,true,   0,10,  0, 0,  0, "" },
//  { eText,true,   0,20,  0, 0,  0, "" },
//  { eText,true,   0,30,  0, 0,  0, "" },
//  { eText,true,   0,40,  0, 0,  0, "" },

  { eText,true,118,  9,  0, 0,  0,"+" },
  { eText,true,118, 46,  0, 0,  0,"-" },
  { eText,true,   0,56,  0, 0,  0,"          >      Set" },
  { eText,true,   0,56,  0, 0,  0," Back" },
  { eText,true,  54,56,  0, 0,  0,"" },
  { eNone,false, 0,0,0,0,  0,NULL },
};

char lcdscreenedit::m_timerTextEdit[128];

static lcdscreenedit editscreen;

void lcdscreenedit::setInputString( char *text )
{
  strcpy(m_timerTextEdit,text);
}

char *lcdscreenedit::getInputString()
{
  return m_timerTextEdit;
}

lcdscreenedit::lcdscreenedit()
  : lcdscreen(strings)
  , m_selectedPos(-1)
  , m_substring("")
{
  lcdscreen::setupScreen(EDIT_SCREEN,&editscreen);
}

lcdscreenedit::~lcdscreenedit()
{
}

void lcdscreenedit::activatedHandler()
{
  if( strlen(m_timerTextEdit)==0 )
  {
    time_t clock = time(NULL);
    struct tm start = *localtime(&clock);
    struct tm stop = start;
    sprintf(m_timerTextEdit,"%02d.%02d.%02d %02d:%02d-%02d:%02d",
            start.tm_mday,start.tm_mon+1,start.tm_year-100,start.tm_hour,start.tm_min,
            stop.tm_hour,stop.tm_min);
  //          strcpy(timerTextEdit,"26.03.15 10:05-10:07");
  }
  m_selectedPos = 0;
  strings[3].text=" Back";
}

keyType lcdscreenedit::keyEventHandler( keyType key )
{
  switch( key )
  {
  case eKeyA:
    if( m_selectedPos>0 )
    {
      m_selectedPos--;
    while( m_selectedPos>0 && m_timerTextEdit[m_selectedPos]==' ' )
      m_selectedPos--;
    if( m_selectedPos==0 )
      strings[3].text=" Back";
    repaint();
    }
    else
    {
      strcpy(m_timerTextEdit,"");
      return eKeyCancel;
    }
    break;
  case eKeyB:
    if( m_selectedPos<strlen(m_timerTextEdit) )
      m_selectedPos++;
    while( m_selectedPos<strlen(m_timerTextEdit) && m_timerTextEdit[m_selectedPos]==' ' )
      m_selectedPos++;
    strings[3].text="  <";
    repaint();
    break;
  case eKeyC:
    return eKeyCancel;
    break;
  case eKeyUp:
    incdecChar(m_timerTextEdit,m_selectedPos,+1);
    repaint();
    break;
  case eKeyDown:
    incdecChar(m_timerTextEdit,m_selectedPos,-1);
    repaint();
    break;
  default:
    break;
  }

  return eKeyNone;
}

void lcdscreenedit::paintEvent()
{
//    LCD_SetFont(0);
//    LCD_PrintXY(0, 0,timerText1);
    LCD_SetFont(1);
    LCD_PrintXY(30,10,substring(m_timerTextEdit,0,9));
    LCD_PrintXY(20,26,substring(m_timerTextEdit,9,11));

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

char *lcdscreenedit::substring( char *str, int start, int len )
{
  strcpy( m_substring, str+start );
  m_substring[len] = 0x0;

  return m_substring;
}

void lcdscreenedit::incdecChar( char *str, int pos, int delta )
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

bool lcdscreenedit::scantimerSetting( const char *source, int srcFormat, struct tm &start, struct tm & stop )
{
  // "15.03.2015 10:05-10:07"
  // "15.03.15 10:05-10:07"
  // "15.03.15  10:05:00"
  char setting[128];

  strcpy(setting,source);

  if( setting[0]=='-' ) return false;

  if( setting[2]!='.' ) return false; else setting[2]=0x0;
  if( setting[5]!='.' ) return false; else setting[5]=0x0;

  switch( srcFormat )
  {
  case editFormatStartStopLong:
    if( setting[10]!=' ' ) return false; else setting[10]=0x0;
    if( setting[13]!=':' ) return false; else setting[13]=0x0;
    if( setting[16]!='-' ) return false; else setting[16]=0x0;
    if( setting[19]!=':' ) return false; else setting[19]=0x0;
    break;
  case editFormatStartStopShort:
    if( setting[8]!=' ' ) return false; else setting[8]=0x0;
    if( setting[11]!=':' ) return false; else setting[11]=0x0;
    if( setting[14]!='-' ) return false; else setting[14]=0x0;
    if( setting[17]!=':' ) return false; else setting[17]=0x0;
    break;
  case editFotmatSettingTime:
    if( setting[8]!=' ' ) return false; else setting[8]=0x0;
    if( setting[9]!=' ' ) return false; else setting[9]=0x0;
    if( setting[12]!=':' ) return false; else setting[12]=0x0;
    if( setting[15]!=':' ) return false; else setting[15]=0x0;
    break;
  }

  start.tm_mday = atoi(setting);
  start.tm_mon = atoi(setting+3) - 1;
  switch( srcFormat )
  {
  case editFormatStartStopLong:
    start.tm_year = atoi(setting+6) - 1900;
    start.tm_hour = atoi(setting+11);
    start.tm_min = atoi(setting+14);
    start.tm_sec = 0;
    break;
  case editFormatStartStopShort:
    start.tm_year = atoi(setting+6) + 100;
    start.tm_hour = atoi(setting+9);
    start.tm_min = atoi(setting+12);
    start.tm_sec = 0;
    break;
  case editFotmatSettingTime:
    start.tm_year = atoi(setting+6) + 100;
    start.tm_hour = atoi(setting+10);
    start.tm_min = atoi(setting+13);
    start.tm_sec = atoi(setting+16);
    break;
  }

  stop = start;

  switch( srcFormat )
  {
  case editFormatStartStopLong:
    stop.tm_hour = atoi(setting+17);
    stop.tm_min = atoi(setting+20);
    break;
  case editFormatStartStopShort:
    stop.tm_hour = atoi(setting+15);
    stop.tm_min = atoi(setting+18);
    break;
  case editFotmatSettingTime:
    break;
  }

  return true;
}
