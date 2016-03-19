// Filename:    lcdscreenselect.h
// Description: retreiving a wav file list and selecting a file out of this list
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include "lcdscreenselect.h"
#include "screenids.h"
#include "lcdscreenmessages.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static char infoLine[32];

static objectinfo strings[] = {
  { eText,true,   0,56,  0, 0,  0,"Cancel  Info   Select" },
  { eText,true,   0,56,  0, 0,  0,infoLine },
  { eNone,false, 0,0,0,0,  0,NULL },
};

static lcdscreenselect selectscreen;

char lcdscreenselect::m_selected[1024];

lcdscreenselect::lcdscreenselect()
  : lcdscreen(strings)
  , m_filecount(0)
  , m_startIdx(0)
  , m_selIdx(0)
  , m_total(1)
  , m_used(1)
  , m_info(false)
{
  strings[1].visible = false;
  lcdscreen::setupScreen(SELECT_SCREEN,&selectscreen,"selectScreen");
}

lcdscreenselect::~lcdscreenselect()
{
}

char *lcdscreenselect::selectedFile()
{
  myprintf("selected: %s\n",m_selected);
  return m_selected;
}
void lcdscreenselect::resetSelectedFile()
{
  strcpy(m_selected,"");
}

void lcdscreenselect::getRemainig(int &hours,int &minutes,int &seconds)
{
  lcdscreenselect *inst = (lcdscreenselect*)lcdscreen::getScreen(SELECT_SCREEN);
  inst->activatedHandler();
  int kbytes = (inst->m_total - inst->m_used);
  seconds = (int)((double)kbytes/172.265625);
  hours = seconds/3600; seconds = seconds % 3600;
  minutes = seconds / 60; seconds = seconds % 60;
}

void lcdscreenselect::activatedHandler()
{
  cleanup();

  char buffer [1024];

#ifdef QT_EMULATION
  const char *file1 = "filelist.txt";
  const char *file2 = "df.txt";
#else
  system("ls -lt /home/pi/usbstick/*.wav > /home/pi/filelist.txt");
  system("df > /home/pi/df.txt");
  const char *file1 = "/home/pi/filelist.txt";
  const char *file2 = "/home/pi/df.txt";
#endif

  FILE *fp = fopen(file1,"r");
  if( fp )
  {
    //-rwxr-xr-x 1 pi pi  288285840 Jul 17 21:59 /home/pi/usbstick/2015-07-17T21-32-45.wav
    m_filecount = 0;
    int mbs = 0;
    while( fgets(buffer,1023,fp) )
    {
      strtok(buffer,"\n");
      int pos1 = 18; while( buffer[pos1]==' ' ) pos1++; // start pos of size col
      int pos2 = pos1; while( buffer[pos2]!=' ' ) pos2++; // end pos of size col
      int pos3 = strlen(buffer)-1; while( buffer[pos3]!='.') pos3--; // end pos of filename (w/o extension)
      buffer[pos3--] = 0x0; while( buffer[pos3]!='/') pos3--; // start pos of filename (w/o path)
      buffer[pos2] = 0x0; m_sizes[m_filecount] = atoi(buffer+pos1);
      m_files[m_filecount] = new char[strlen(buffer+pos3+1)+1];
      strcpy(m_files[m_filecount],buffer+pos3+1);
      myprintf("%s (%d bytes)\n",m_files[m_filecount],m_sizes[m_filecount]);
      mbs += m_sizes[m_filecount] / 1024;
      m_filecount++;
    }
    myprintf("total used %d kB\n",mbs);
    fclose(fp);
  }
  fp = fopen(file2,"r");
  if( fp )
  {
    //dev/sda1        7699456 5760528   1938928  75% /home/pi/usbstick
    m_total = 0;
    m_used = 0;
    while( fgets(buffer,1023,fp) )
    {
      strtok(buffer,"\n");
      if( strstr(buffer,"usbstick") )
      {
        myprintf("%s\n",buffer);
        int pos1 = 9; while( buffer[pos1]==' ' ) pos1++; // start pos of size
        int pos2 = pos1; while( buffer[pos2]!=' ' ) pos2++; // end pos of size
        int pos3 = pos2; while( buffer[pos3]==' ') pos3++; // start pos of used
        int pos4 = pos3; while( buffer[pos4]!=' ') pos4++; // end pos of used
        buffer[pos2] = 0x0; m_total = atoi(buffer+pos1);
        buffer[pos4] = 0x0; m_used = atoi(buffer+pos3);
      }
    }
    fclose(fp);
    myprintf("total %d, used %d kB\n",m_total,m_used);
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
    if( m_info )
    {
      m_info = false;
      strings[0].visible = true;
      strings[1].visible = false;
      repaint();
    }
    else
    {
      strcpy(m_selected,"");
      return eKeyCancel;
    }
    break;
  case eKeyB:
    if( m_info )
      ;
    else
    {
      m_info = true;
      strings[0].visible = false;
      strings[1].visible = true;
    }
    repaint();
    break;
  case eKeyC:
    if( m_info )
    {
      lcdscreenQuestion *quest = (lcdscreenQuestion*)(getScreen(MESSAGE_SCREEN));
      quest->setMessage("Delete recording");
      quest->setButtons(" Yes           Cancel");
      activateScreen(MESSAGE_SCREEN);
    }
    else
    {
      strcpy(m_selected,"/home/pi/usbstick/");
      strcat(m_selected,m_files[m_selIdx]);
      strcat(m_selected,".wav");
      return eKeyCancel;
    }
    break;
  case eKeyCancel:
    if( lcdscreenQuestion::YesClicked() )
    {
      strcpy(m_selected,"/home/pi/usbstick/");
      strcat(m_selected,m_files[m_selIdx]);
      strcat(m_selected,".wav");
      if( remove(m_selected)==0 )
        m_filecount--;
      if( m_selIdx>=m_filecount ) m_selIdx=m_filecount-1;
      strcpy(m_selected,"");
      activatedHandler();
      m_info = false;
      strings[0].visible = true;
      strings[1].visible = false;
      repaint();
    }
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
      char buffer[32];
      if( m_info )
      {
        int bytes = m_sizes[m_startIdx+i];
        int kbytes = bytes/1024;
        int seconds = bytes/2/2/44100;
        int hours = seconds/3600; seconds = seconds % 3600;
        int mins = seconds / 60; seconds = seconds % 60;
        sprintf(buffer,"%02d:%02d:%02d %4d MB %2d%%",
                hours,mins,seconds,
              kbytes/1024,
              kbytes*100/m_total);
        LCD_PrintXY(3, 2+i*10,buffer);
      }
      else
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

  sprintf(infoLine,"Cancel %2d%%free Delete",(m_total-m_used)*100/m_total);
}

void lcdscreenselect::cleanup()
{
}
