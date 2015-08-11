// Filename:    lcdscreencpu
// Description: shows the cpu temperature histoty
//
// Open Source Licensing GPL 3
//
// Author:      Martin Steppuhn, www.emsystech.de
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lcdscreencpu.h"
#include "screenids.h"

char	TempString[32];
uint16	DemoMem[256];

static lcdscreencpu cpuscreen;

lcdscreencpu::lcdscreencpu()
  : lcdscreen(NULL)
{
  uint8 i;
  for(i=0;i<128;i++)  DemoMem[i] = 0;

  secTimerHandler(0);
  setupScreen(CPUTEMP_SCREEN,this);
}

lcdscreencpu::~lcdscreencpu()
{
}

char *lcdscreencpu::getTemp()
{
  return TempString;
}

void lcdscreencpu::paintEvent()
{
  lcdscreen::paintEvent();
  uint16	i,y;

  //	LCD_ClearScreen();
  LCD_SetPenColor(1);
  LCD_SetFont(1);
  LCD_PrintXY(40,0,"CPU:");
  LCD_PrintXY(80,0,TempString);

  LCD_SetFont(0);
  LCD_PrintXY(0,0, "60-");
  LCD_PrintXY(0,18,"50-");
  LCD_PrintXY(0,37,"40-");
  LCD_PrintXY(0,56,"30-");
  LCD_DrawLine(15,0,15,63);

  for(i=16;i<128;i++)
  {
    y = DemoMem[127-i];

    if(y > 290)
    {
      y = ((y - 290) / 5);
      y = 64 - y;
      LCD_PutPixel(i,y,1);
      LCD_PutPixel(i,y+1,1);
    }
  }
}

keyType lcdscreencpu::secTimerHandler(struct tm */*result*/)
{
  FILE *fp;
  unsigned int temp;
  uint16 i;

  TempString[0] = 0;

  fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");		// open as file
  if(fp != NULL)
  {
    fgets(TempString,32,fp);			// get line
    fclose(fp);
  }
  else
    strcpy(TempString,"441456");

  temp = 0;
  if(TempString[0])
  {
    TempString[3]=0;	// end at 1/10 C
    sscanf(TempString,"%u",&temp);
    TempString[3]=TempString[2]; TempString[2]=','; TempString[4]=0/*' '*/; TempString[5]='C'; TempString[6]=0;
  }

  for(i=126;i>0;i--)  DemoMem[i+1] = DemoMem[i];
  DemoMem[1] = DemoMem[0];
  DemoMem[0] = temp;

  repaint();

  return eKeyNone;
}

keyType lcdscreencpu::keyEventHandler( keyType key )
{
  switch( key )
  {
  case eKeyA:
  case eKeyB:
  case eKeyC:
  case eKeyD:
    return eKeyCancel;
    break;
  default:
    break;
  }

  return eKeyNone;
}
