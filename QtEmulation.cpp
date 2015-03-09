// Filename:    QtEmulation
// Description: Raspi LCD Emulation using QT 4.8.6
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include "QtEmulation.h"
#include "lcdscreen.h"

#include <QRadioButton>
#include <QPainter>
#include <QPixmap>
#include <QMessageBox>

static RaspiLcdWidget *m_window = NULL;

#define marginx  15
#define marginy  15

RaspiLcdWidget::RaspiLcdWidget( QWidget *parent )
  : QWidget(parent)
  , m_image(128,64,QImage::Format_Mono)
{
  QRadioButton *butA = new QRadioButton(this);
  butA->move(24+marginx,160+marginy);
  butA->setCheckable(false);
  connect(butA,SIGNAL(clicked()),this,SLOT(buttonApressed()));
  QRadioButton *butB = new QRadioButton(this);
  butB->move(120+marginx,160+marginy);
  butB->setCheckable(false);
  connect(butB,SIGNAL(clicked()),this,SLOT(buttonBpressed()));
  QRadioButton *butC = new QRadioButton(this);
  butC->move(216+marginx,160+marginy);
  butC->setCheckable(false);
  connect(butC,SIGNAL(clicked()),this,SLOT(buttonCpressed()));
  QRadioButton *butD = new QRadioButton(this);
  butD->move(300+marginx,160+marginy);
  butD->setCheckable(false);
  connect(butD,SIGNAL(clicked()),this,SLOT(buttonDpressed()));

  QRadioButton *butUp = new QRadioButton(this);
  butUp->move(300+marginx,10+marginy);
  butUp->setCheckable(false);
  connect(butUp,SIGNAL(clicked()),this,SLOT(buttonUppressed()));
  QRadioButton *butDown = new QRadioButton(this);
  butDown->move(300+marginx,96+marginy);
  butDown->setCheckable(false);
  connect(butDown,SIGNAL(clicked()),this,SLOT(buttonDownpressed()));

  resize(350+2*marginx,200+2*marginy);

  startTimer(50);
}

RaspiLcdWidget::~RaspiLcdWidget()
{
}

void RaspiLcdWidget::paintEvent( QPaintEvent */*event*/ )
{
  QPainter p(this);
  p.drawPixmap(marginx,marginy,256,128,QPixmap::fromImage(m_image));
}
void RaspiLcdWidget::timerEvent ( QTimerEvent */*event*/ )
{
  lcdscreen::updateDisplay();
  lcdscreen::updateTimer();
}

void RaspiLcdWidget::buttonApressed()
{
  lcdscreen::keyPressed(eKeyA);
}
void RaspiLcdWidget::buttonBpressed()
{
  lcdscreen::keyPressed(eKeyB);
}
void RaspiLcdWidget::buttonCpressed()
{
  lcdscreen::keyPressed(eKeyC);
}
void RaspiLcdWidget::buttonDpressed()
{
  lcdscreen::keyPressed(eKeyD);
}
void RaspiLcdWidget::buttonUppressed()
{
  lcdscreen::keyPressed(eKeyUp);
}
void RaspiLcdWidget::buttonDownpressed()
{
  lcdscreen::keyPressed(eKeyDown);
}

///////////////////////////////////////////////////////////////////

void QtEmulation_Init()
{
  m_window = new RaspiLcdWidget(0);
  m_window->show();
}

void QtEmulation_Exit()
{
  delete m_window;
  m_window = NULL;
}

/////////////////////////////////////////////////////////////////////

QColor m_pencol = Qt::color0;
QColor m_fillcol = Qt::color1;

void Qt_ClearScreen(void)
{
  QPainter p(&m_window->m_image);
  p.fillRect(0,0,128,64,m_fillcol);
}

void Qt_SetPenColor(uint8 c)
{
  m_pencol = c ? Qt::color0 : Qt::color1;
}

void Qt_SetFillColor(int8 c)
{
  m_fillcol = c ? Qt::color0 : Qt::color1;
}

void Qt_SetContrast(uint8 /*contrast*/)
{
}

void Qt_PutPixel(uint8 x,uint8 y,uint8 color)
{
  m_window->m_image.setPixel(x, y, color ? Qt::color1 : Qt::color0);
  //m_window->repaint();
}

void Qt_DrawLine(uint8 x0,uint8 y0,uint8 x1,uint8 y1)
{
  QPainter p(&m_window->m_image);
  p.setPen(m_pencol);
  p.drawLine(x0,y0,x1,y1);
}

void Qt_DrawCircle(uint8 x0,uint8 y0,uint8 radius)
{
  QPainter p(&m_window->m_image);
  p.setPen(m_pencol);
  p.drawEllipse(x0-radius/2,y0-radius/2,x0+radius/2,y0+radius/2);
}

void Qt_DrawEllipse(int xm, int ym, int a, int b)
{
  QPainter p(&m_window->m_image);
  p.setPen(m_pencol);
  p.drawEllipse(xm-a/2,ym-b/2,xm+a/2,ym+b/2);
}

void Qt_DrawRect(uint8 x0,uint8 y0,uint8 x1,uint8 y1,uint8 /*line*/)
{
  QPainter p(&m_window->m_image);
  p.setPen(m_pencol);
  p.drawRect(x0,y0,x1-x0+1,y1-y0+1);
}

/*void Qt_DrawBitmap(uint8 x0,uint8 y0,const uint8 *bmp)
{
}*/

void Qt_WriteFramebuffer(void)
{
  m_window->repaint();
}
