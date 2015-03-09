// Filename:    QtEmulation
// Description: Raspi LCD Emulation using QT 4.8.6
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#ifndef QTEMULATION_H
#define QTEMULATION_H

#include "std_c.h"

#ifdef __cplusplus
#include <QApplication>
#include <QWidget>

class RaspiLcdWidget : public QWidget
{
  Q_OBJECT

public:
  RaspiLcdWidget( QWidget *parent );
  virtual ~RaspiLcdWidget();

  QImage m_image;

protected:
  virtual void paintEvent( QPaintEvent *event );
  virtual void timerEvent ( QTimerEvent * event );

private slots:
  void buttonApressed();
  void buttonBpressed();
  void buttonCpressed();
  void buttonDpressed();
  void buttonUppressed();
  void buttonDownpressed();
};

extern "C"
{
#endif

void QtEmulation_Init();
void QtEmulation_Exit();

void Qt_ClearScreen(void);
void Qt_SetPenColor(uint8 c);
void Qt_SetFillColor(int8 c);
//void Qt_SetFont(uint8 f);
void Qt_SetContrast(uint8 contrast);

void Qt_PutPixel(uint8 x,uint8 y,uint8 color);
void Qt_DrawLine(uint8 x0,uint8 y0,uint8 x1,uint8 y1);
void Qt_DrawCircle(uint8 x0,uint8 y0,uint8 radius);
void Qt_DrawEllipse(int xm, int ym, int a, int b);
void Qt_DrawRect(uint8 x0,uint8 y0,uint8 x1,uint8 y1,uint8 line);
void Qt_PrintXY(uint8 x,uint8 y,const char *s);
void Qt_DrawBitmap(uint8 x0,uint8 y0,const uint8 *bmp);

void Qt_Init(void);
void Qt_WriteFramebuffer(void);

#ifdef __cplusplus
}
#endif

#endif // QTEMULATION_H
