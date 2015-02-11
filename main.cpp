//=== Includes =====================================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <termios.h>
#include <stdio.h>

extern "C"
{
#include "std_c.h"
#include "bcm2835.h"
#include "raspilcd.h"
#include "lcd.h"
}

#include "lcdscreenmain.h"

//=== Preprocessing directives (#define) ===========================================================

//=== Type definitions (typedef) ===================================================================

//=== Global constants =============================================================================

//=== Global variables =============================================================================

//=== Local constants  =============================================================================

#include <signal.h>

#include "bmp_raspi.inc"
#include "bmp_men.inc"

//=== Local variables ==============================================================================

uint16  DemoCount;
uint8	DemoView;
char	TempString[32];
uint16	DemoMem[256];

//=== Local function prototypes ====================================================================

//--------------------------------------------------------------------------------------------------
// Name:      Demo Functions
// Function:  
//            
// Parameter: 
// Return:    
//--------------------------------------------------------------------------------------------------
void DemoLogo(void)
{
	LCD_ClearScreen();
	LCD_DrawBitmap(0,0,bmp_raspi);		
	LCD_SetFont(0);	
	LCD_PrintXY(70,4 ,"Raspi-LCD");		
	LCD_PrintXY(75,14,"Project");
	LCD_PrintXY(68,32,"powered by");
	LCD_PrintXY(70,42,"Emsystech");
	LCD_PrintXY(62,52,"Engineering");
}

void DemoText(void)
{	
  LCD_ClearScreen();
	LCD_SetFont(1);	
	LCD_PrintXY(30,0,"Raspi-LCD");
	LCD_PrintXY(0,12,"www.emsystech.de");
	LCD_SetFont(0);	
	LCD_PrintXY(8,29,"128 x 64 Pixel (BW)");
	LCD_PrintXY(6,38,"White LED Backlight");
	LCD_PrintXY(4,47,"8 Lines with 21 Char ");
	LCD_PrintXY(4,56,"in the smallest Font ");
}

void DemoVector(void)
{
  LCD_ClearScreen();
	LCD_SetFillColor(1);	
	LCD_DrawRect(110,20,120,60,1);
	LCD_DrawLine(0,0,127,0);
	LCD_DrawLine(0,0,127,16);
	LCD_DrawLine(0,0,127,32);
	LCD_DrawLine(0,0,127,48);
	LCD_DrawLine(0,0,127,63);
	LCD_SetPenColor(1);
	LCD_DrawCircle(63,31,31);
	LCD_DrawCircle(8,50,5);
	LCD_DrawEllipse(80,40, 30,10);
	LCD_SetFillColor(0);	
	LCD_DrawRect(12,20,40,40,3);
	LCD_SetFillColor(-1);	
	LCD_DrawRect(30,50,60,60,1);
}

void LogCpuTemperature(void)
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
	
	temp = 0;
	if(TempString[0])
	{
		TempString[3]=0;	// end at 1/10 C
		sscanf(TempString,"%u",&temp);
//		printf("%u\r\n",temp);
    TempString[3]=TempString[2]; TempString[2]='.'; TempString[4]='?'; TempString[5]='C'; TempString[6]=0;
	}
	
	for(i=126;i>0;i--)  DemoMem[i+1] = DemoMem[i];
	DemoMem[1] = DemoMem[0];
	DemoMem[0] = temp;
}	

void DemoCpuTemperature(void)
{
  uint16	i,y;
	
	LCD_ClearScreen();
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

void DemoCpuTemperatureInit(void)
{
	uint8 i;
	for(i=0;i<128;i++)  DemoMem[i] = 0;
}

void DemoBubbles(void)
{
  LCD_ClearScreen();
	LCD_SetPenColor(1);
	
	if(DemoMem[200])	{ if(DemoMem[201] > 16)	DemoMem[201]--; else	DemoMem[200] = 0;	}
		else		{ if(DemoMem[201] <48)	DemoMem[201]++; else	DemoMem[200] = 1;	}
	DemoMem[202] = ((63 - DemoMem[201]) < DemoMem[201]) ? (63 - DemoMem[201]) : DemoMem[201];	
	DemoMem[202] = (DemoMem[202] > 20) ? 20 : DemoMem[202];	
	LCD_DrawEllipse(28,DemoMem[201],20+20-DemoMem[202],DemoMem[202]);	

	if(DemoMem[203])	{ if(DemoMem[204] > 14)	DemoMem[204]--; else	DemoMem[203] = 0;	}
		else		{ if(DemoMem[204] <50)	DemoMem[204]++; else	DemoMem[203] = 1;	}
	DemoMem[205] = ((63 - DemoMem[204]) < DemoMem[204]) ? (63 - DemoMem[204]) : DemoMem[204];	
	DemoMem[205] = (DemoMem[205] > 20) ? 20 : DemoMem[205];	
	LCD_DrawEllipse(65,DemoMem[204],22+10-DemoMem[205],DemoMem[205]);	

	if(DemoMem[206])	{ if(DemoMem[207] > 10)	DemoMem[207]--; else	DemoMem[206] = 0;	}
		else		{ if(DemoMem[207] <54)	DemoMem[207]++; else	DemoMem[206] = 1;	}
	DemoMem[208] = ((63 - DemoMem[207]) < DemoMem[207]) ? (63 - DemoMem[207]) : DemoMem[207];	
	DemoMem[208] = (DemoMem[208] > 15) ? 15 : DemoMem[208];	
	LCD_DrawEllipse(102,DemoMem[207],15+20-DemoMem[208],DemoMem[208]);	
}

void DemoBubblesInit(void)
{
	DemoMem[200] = 1;
	DemoMem[201] = 10;
	DemoMem[203] = 0;
	DemoMem[204] = 40;
	DemoMem[206] = 1;
	DemoMem[207] = 40;
}

void DemoFont(void)
{
  LCD_ClearScreen();
	LCD_SetFont(0);		LCD_PrintXY(0,0, "Font 0");
	LCD_SetFont(1);		LCD_PrintXY(0,8, "Font 1");
	LCD_SetFont(2);		LCD_PrintXY(0,23,"Font 2");
	LCD_SetFont(3);		LCD_PrintXY(0,39,"Font 3");
}

void DemoBitmap(void)
{
  LCD_DrawBitmap(0,0,bmp_men);
	LCD_SetFont(1);		LCD_PrintXY(6,0, "Bitmap");
}
 
int getch() {
    static int ch = -1, fd = 0;
    struct termios neu, alt;
    fd = fileno(stdin);
    tcgetattr(fd, &alt);
    neu = alt;
    neu.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(fd, TCSANOW, &neu);
    ch = getchar();
    tcsetattr(fd, TCSANOW, &alt);
    return ch;
}	
 
int kbhit(void) {
        struct termios term, oterm;
        int fd = 0;
        int c = 0;
        tcgetattr(fd, &oterm);
        memcpy(&term, &oterm, sizeof(term));
        term.c_lflag = term.c_lflag & (!ICANON);
        term.c_cc[VMIN] = 0;
        term.c_cc[VTIME] = 1;
        tcsetattr(fd, TCSANOW, &term);
        c = getchar();
        tcsetattr(fd, TCSANOW, &oterm);
        if (c != -1)
        ungetc(c, stdin);
        return ((c != -1) ? 1 : 0);
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

typedef void (*sighandler_t)(int);
extern "C" sighandler_t signal(int signum, sighandler_t handler);

volatile bool running = true;

void terminateHandler()
{
  running = false;
}

int main(int argc, char **argv)
{
  int Contrast;

  if( argc>1 )
  {
    if( strcmp(argv[1],"-debug")==0 )
      lcdscreen::setDebuMode(true);
  }

  Contrast = 9;

  signal(SIGTERM, (sighandler_t)terminateHandler);
  // avoid zombie processes...
  if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
  {
    perror(0);
    exit(1);
  }

  printf("RaspiLCD Demo V1.0.0 by Martin Steppuhn [" __DATE__ " " __TIME__"]\n");
	printf("RaspberryHwRevision=%i\r\n",GetRaspberryHwRevision());
		
	if(!RaspiLcdHwInit()) { printf("RaspiLcdHwInit() failed!\r\n"); return 1; }
	LCD_Init();			// Init Display
	SetBacklight(1);	// Turn Backlight on
	
	DemoView = 0;
	DemoCpuTemperatureInit();
	DemoBubblesInit();

  lcdscreenmain mainscreen;

  lcdscreen::activateScreen(&mainscreen);

  while(running)
	{
		DemoCount++;
    SleepMs(100); // Raspi B Rev. 2: this is about 16-17 times a second with drawing, 19-20 without...
		UpdateButtons();

    if( (DemoCount % 20) == 0 ) LogCpuTemperature();
		
    if(Button) printf("Buttons: %02X (%02X) Contrast=%i\r\n",Button,ButtonPressed,Contrast);

    lcdscreen::updateDisplay();
    lcdscreen::updateTimer();

    if(BUTTON_PRESSED_UP || BUTTON_PRESSED_DOWN)
		{
      printf("Button1\n");
      if( DemoView == 0 )
      {
        if(BUTTON_PRESSED_UP) lcdscreen::keyPressed(eKeyUp);
        if(BUTTON_PRESSED_DOWN) lcdscreen::keyPressed(eKeyDown);
      }
      else
      {
        if(BUTTON_PRESSED_UP   && (Contrast < 20))  Contrast++;
        if(BUTTON_PRESSED_DOWN && (Contrast > 0))   Contrast--;
        LCD_SetContrast(Contrast);
      }
		}

    if(BUTTON_PRESSED_C)
    {
      printf("Button2\n");
      if( DemoView == 0 )
      {
        lcdscreen::keyPressed(eKeyC);
      }
    }

    if(BUTTON_PRESSED_D)
		{
      printf("Button3\n");
      lcdscreen::keyPressed(eKeyD);
    }

    if( DemoView == 0 )
    {
      if(BUTTON_PRESSED_A )
      {
        printf("Button4a\n");
        lcdscreen::keyPressed(eKeyA);
      }
      if(BUTTON_PRESSED_B )
      {
        printf("Button4b\n");
        DemoView = lcdscreen::keyPressed(eKeyB);
        if( DemoView>0 ) lcdscreen::activateScreen(0);
      }
    }
    else
    {
      if(BUTTON_PRESSED_A  && DemoView)
      {
        printf("Button5\n");
        DemoView--;
        if( DemoView == 0 )
          lcdscreen::activateScreen(&mainscreen);
      }
      if(BUTTON_PRESSED_B && (DemoView < 6))
      {
        printf("Button6\n");
        DemoView++;
      }
    }
	
//		if(     DemoView == 0)	 DemoLogo();
/*    if( DemoView == 0 )
      lcdscreen::activateScreen(&mainscreen);
    else
      lcdscreen::activateScreen(0);*/

    if(DemoView == 1)	 { if((DemoCount & 3) == 0) DemoCpuTemperature(); }
		else if(DemoView == 2)  DemoBitmap();
		else if(DemoView == 3)  DemoFont();
		else if(DemoView == 4)	 DemoVector();
		else if(DemoView == 5)	 DemoBubbles();
    else if(DemoView == 6)	 DemoText();
					
    LCD_WriteFramebuffer();
  }

  LCD_Init();			// Init Display
  SetBacklight(0);	// Turn Backlight off
  DemoView = 0;
  return(0);
}
