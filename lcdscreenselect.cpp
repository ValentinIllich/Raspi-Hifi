// Filename:    lcdscreenselect.h
// Description: retreiving a wav file list and selecting a file out of this list
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include "lcdscreenselect.h"
#include "screenids.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static objectinfo strings[] = {
  { eText,true,   0,56,  0, 0,  0,"Cancel         Select" },
  { eNone,false, 0,0,0,0,  0,NULL },
};

static lcdscreenselect selectscreen;

char lcdscreenselect::m_selected[1024];

lcdscreenselect::lcdscreenselect()
  : lcdscreen(strings)
  , m_filecount(0)
  , m_startIdx(0)
  , m_selIdx(0)
{
  lcdscreen::setupScreen(SELECT_SCREEN,&selectscreen);
}

lcdscreenselect::~lcdscreenselect()
{
}

char *lcdscreenselect::selectedFile()
{
  return m_selected;
}
void lcdscreenselect::resetSelectedFile()
{
  strcpy(m_selected,"");
}

void lcdscreenselect::activatedHandler()
{
  cleanup();

  system("ls -lt /home/pi/usbstick/*.wav > /home/pi/filelist.txt");

  FILE *fp = fopen("/home/pi/filelist.txt","r");
  if( fp )
  {
    char buffer [1024];

    m_filecount = 0;
    while( fgets(buffer,1023,fp) )
    {
      strtok(buffer,"\n");
      int pos = strlen(buffer)-1;
      while( buffer[pos]!='/') pos--;
      m_files[m_filecount] = new char[strlen(buffer+pos+1)+1];
      strcpy(m_files[m_filecount++],buffer+pos+1);
    }
    fclose(fp);
  }
}

keyType lcdscreenselect::secTimerHandler(struct tm */*result*/)
{
  return eKeyNone;
}

keyType lcdscreenselect::keyEventHandler( keyType key )
{
  switch( key )
  {
  case eKeyA:
    strcpy(m_selected,"");
    return eKeyCancel;
    break;
  case eKeyC:
    strcpy(m_selected,"/home/pi/usbstick/");
    strcat(m_selected,m_files[m_selIdx]);
    return eKeyCancel;
    break;
  case eKeyUp:
    m_selIdx--;
    if( m_selIdx<0 ) m_selIdx=0;
    if( m_startIdx>m_selIdx )
    {
      m_startIdx -= 5;
      if( m_startIdx<0 ) m_startIdx=0;
    }
    repaint();
    break;
  case eKeyDown:
    m_selIdx++;
    if( m_selIdx>=(m_startIdx+5) )
      m_startIdx+=5;
    if( m_selIdx>=m_filecount ) m_selIdx=m_filecount-1;
    repaint();
    break;
  default:
    break;
  }

  return eKeyNone;
}

void lcdscreenselect::paintEvent()
{
  LCD_SetFont(0);
  for( int i=0; i<5; i++ )
  {
    if( (m_startIdx+i)<m_filecount )
    {
      LCD_PrintXY(3, 2+i*10,m_files[m_startIdx+i]);
      if( (m_startIdx+i)==m_selIdx )
      {
        int x = 0, y = 10*i, dx = 127, dy = 10;
        LCD_SetPenColor(1);
        LCD_DrawLine(x,y,x+dx,y);
        LCD_DrawLine(x+dx,y,x+dx,y+dy);
        LCD_DrawLine(x+dx,y+dy,x,y+dy);
        LCD_DrawLine(x,y,x,y+dy);
      }
    }
  }
}

void lcdscreenselect::cleanup()
{
}
