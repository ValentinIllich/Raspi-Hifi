// Filename:    lcdscreenedit
// Description: allows editing of a date / time data set (as string) in three different forms:
//              "15.03.2015 10:05-10:07" is start / stop of a timer read from file
//              "15.03.15 10:05-10:07"   is start / stop of a timer by edit
//              "15.03.15  10:05:00"     is system clock
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lcdscreenrename.h"
#include "screenids.h"

#define ROW_NONE      0
#define ROW1_SELECTED 1
#define ROW2_SELECTED 2
#define ROW3_SELECTED 3
#define ROW4_SELECTED 4

#define COL_NONE      0
#define COL1_SELECTED 1
#define COL2_SELECTED 2
#define COL3_SELECTED 3
#define COL4_SELECTED 9
#define COL5_SELECTED 10
#define COL6_SELECTED 11
#define COL7_SELECTED 16
#define COL8_SELECTED 17
#define COL9_SELECTED 18

static int m_selected_row = ROW_NONE;
static int m_selected_col = COL_NONE;

const char *m_row1Up = ".123.....456....789";
const char *m_row2Up = ".0AB.....CDE....FGH";
const char *m_row3Up = ".IJK.....LMN....OPQ";
const char *m_row4Up = ".RST.....UVW....XYZ";

const char *m_row1Low = ".- _.....   ....   ";
const char *m_row2Low = "..ab.....cde....fgh";
const char *m_row3Low = ".ijk.....lmn....opq";
const char *m_row4Low = ".rst.....uvw....xyz";

const char *m_row1 = m_row1Up;
const char *m_row2 = m_row2Up;
const char *m_row3 = m_row3Up;
const char *m_row4 = m_row4Up;

const char *m_sel1  = "                    ";
const char *m_sel2  = "             1-9/0-H";
const char *m_sel3  = "             I-Q/R-Z";
const char *m_sel4  = "Cancel   Set   Delete";

static lcdscreenRename editscreen;

lcdscreenRename::lcdscreenRename()
  : lcdscreen(NULL)
  , m_startingSeconds(-1)
  , m_entered(false)
  , m_edit_col(0)
  , m_actrow(NULL)
{
  lcdscreen::setupScreen(RENAME_SCREEN,&editscreen,"editScreen");
  strcpy(m_currentInput,"");
}

lcdscreenRename::~lcdscreenRename()
{
}

void lcdscreenRename::setName(char *name)
{
  strcpy(m_currentInput,name);
  m_edit_col = strlen(name);
}

bool lcdscreenRename::SetClicked()
{
  return m_entered;
}

char *lcdscreenRename::getNewName()
{
  return m_currentInput;
}

keyType lcdscreenRename::keyEventHandler( keyType key )
{
  m_entered = false;

  switch( key )
  {
  case eKeyA:
    switch( m_selected_row )
    {
    case ROW1_SELECTED:
    case ROW2_SELECTED:
    case ROW3_SELECTED:
    case ROW4_SELECTED:
      switch( m_selected_col )
      {
      case COL1_SELECTED:
        m_selected_col = COL2_SELECTED;
        break;
      case COL2_SELECTED:
        m_selected_col = COL3_SELECTED;
        break;
      default:
        m_selected_col = COL1_SELECTED;
        break;
      }
      m_startingSeconds = 0;
      break;
    default:
      return eKeyCancel;
      break;
    }
    repaint();
    break;
  case eKeyB:
    switch( m_selected_row )
    {
    case ROW1_SELECTED:
    case ROW2_SELECTED:
    case ROW3_SELECTED:
    case ROW4_SELECTED:
      switch( m_selected_col )
      {
      case COL4_SELECTED:
        m_selected_col = COL5_SELECTED;
        break;
      case COL5_SELECTED:
        m_selected_col = COL6_SELECTED;
        break;
      default:
        m_selected_col = COL4_SELECTED;
        break;
      }
      break;
    default:
      m_entered = true;
      return eKeyCancel;
      m_startingSeconds = 0;
      break;
    }
    repaint();
    break;
  case eKeyC:
    switch( m_selected_row )
    {
    case ROW1_SELECTED:
    case ROW2_SELECTED:
    case ROW3_SELECTED:
    case ROW4_SELECTED:
      switch( m_selected_col )
      {
      case COL7_SELECTED:
        m_selected_col = COL8_SELECTED;
        break;
      case COL8_SELECTED:
        m_selected_col = COL9_SELECTED;
        break;
      default:
        m_selected_col = COL7_SELECTED;
        break;
      }
      repaint();
      m_startingSeconds = 0;
      break;
    default:
      if( m_edit_col>0 )
      {
        m_edit_col--;
        m_currentInput[m_edit_col] = 0x0;
      }
      break;
    }
    repaint();
    break;
  case eKeyUp:
    switch( m_selected_row )
    {
    case ROW1_SELECTED:
      m_selected_row = ROW2_SELECTED;
      break;
    default:
      m_selected_row = ROW1_SELECTED;
      break;
    }
    m_startingSeconds = 0;
    m_selected_col = COL_NONE;
    repaint();
    break;
  case eKeyDown:
    switch( m_selected_row )
    {
    case ROW3_SELECTED:
      m_selected_row = ROW4_SELECTED;
      break;
    default:
      m_selected_row = ROW3_SELECTED;
      break;
    }
    m_startingSeconds = 0;
    m_selected_col = COL_NONE;
    repaint();
    break;
  case eKeyD:
//    m_selected_row = ROW_NONE;
//    m_selected_col = COL_NONE;
    if( m_selected_col!=COL_NONE )
    {
      m_currentInput[m_edit_col++] = m_actrow[m_selected_col];
      repaint();
      return eKeyCancel;
    }
    else if( m_selected_row!=ROW_NONE )
    {
      m_startingSeconds = 0;
      if( m_row1==m_row1Up)
      {
        m_row1 = m_row1Low;
        m_row2 = m_row2Low;
        m_row3 = m_row3Low;
        m_row4 = m_row4Low;
      }
      else
      {
        m_row1 = m_row1Up;
        m_row2 = m_row2Up;
        m_row3 = m_row3Up;
        m_row4 = m_row4Up;
      }
      repaint();
      return eKeyCancel;
    }
    break;
  default:
    break;
  }

  return eKeyNone;
}

void lcdscreenRename::paintEvent()
{
  LCD_SetFont(0);
  LCD_SetPenColor(1);

  int x = 0, y = 0, dx = 127, dy = 10;

  switch( m_selected_row )
  {
  case ROW_NONE:
    LCD_PrintXY(0, 2,m_sel1);
    LCD_PrintXY(0,12,m_sel2);
    LCD_PrintXY(0,45,m_sel3);
    LCD_PrintXY(0,55,m_sel4);
    break;
  default:
    LCD_PrintXY(0, 2,m_row1);
    LCD_PrintXY(0,12,m_row2);
    LCD_PrintXY(0,45,m_row3);
    LCD_PrintXY(0,55,m_row4);
    break;
  }
  switch( m_selected_row )
  {
  case ROW1_SELECTED:
    y = 0;
    m_actrow = m_row1;
    break;
  case ROW2_SELECTED:
    y = 10;
    m_actrow = m_row2;
    break;
  case ROW3_SELECTED:
    y = 43;
    m_actrow = m_row3;
    break;
  case ROW4_SELECTED:
    y = 53;
    m_actrow = m_row4;
    break;
  default:
    break;
  }
  switch( m_selected_col )
  {
  case COL_NONE:
    if( m_selected_row!=ROW_NONE )
    {
/*      LCD_SetPenColor(1);
      LCD_DrawEllipse(20,20,5,5);*/
      LCD_PrintXY(20*6,45,"A");
      LCD_PrintXY(20*6,55,"a");
    }
    break;
  default:
    x = m_selected_col*6; dx = 6;
    LCD_PrintXY(20*6,55,"*");
    break;
  }

  if( m_selected_row!=ROW_NONE )
  {
    LCD_DrawLine(x,y,x+dx,y);
    LCD_DrawLine(x+dx,y,x+dx,y+dy);
    LCD_DrawLine(x+dx,y+dy,x,y+dy);
    //LCD_DrawLine(x,y+dy,x,y); // does not really work ?
    LCD_DrawLine(x,y,x,y+dy);
  }

  LCD_PrintXY(0,28,m_currentInput);
  x = m_edit_col*6; y = 28-2; dx = 6; dy = 10;

  LCD_DrawLine(x,y,x+dx,y);
    LCD_DrawLine(x+dx,y,x+dx,y+dy);
    LCD_DrawLine(x+dx,y+dy,x,y+dy);
    //LCD_DrawLine(x,y+dy,x,y); // does not really work ?
    LCD_DrawLine(x,y,x,y+dy);
}

keyType lcdscreenRename::secTimerHandler(struct tm *result)
{
  if( m_startingSeconds==0 )
    m_startingSeconds = lcdscreen::toTimeInSeconds(result);

  int actualTime = lcdscreen::toTimeInSeconds(result);

  if( (m_startingSeconds>=0) && ((actualTime-m_startingSeconds)>5) )
  {
    m_selected_row = ROW_NONE;
    m_selected_col = COL_NONE;
    repaint();
    //return eKeyCancel;
  }

  return eKeyNone;
}
