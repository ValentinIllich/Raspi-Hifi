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
#include <QClipboard>
#include <QTextEdit>

static RaspiLcdWidget *m_window = NULL;

#define marginx  15
#define marginy  15

QColor m_background = Qt::lightGray;

RaspiLcdWidget::RaspiLcdWidget( QWidget *parent )
  : QWidget(parent)
  , m_image(128,64,QImage::Format_Mono)
  , m_keyPressed(eKeyNone)
{
  QRgb value;
  value = m_background.rgb();
  m_image.setColor(0, value);
  value = QColor(Qt::black).rgb();
  m_image.setColor(1, value);

  QRadioButton *butA = new QRadioButton(this);
  butA->move(24+marginx,160+marginy);
  butA->setCheckable(false);
  registerKey(butA,eKeyA);

  QRadioButton *butB = new QRadioButton(this);
  butB->move(120+marginx,160+marginy);
  butB->setCheckable(false);
  registerKey(butB,eKeyB);

  QRadioButton *butC = new QRadioButton(this);
  butC->move(216+marginx,160+marginy);
  butC->setCheckable(false);
  registerKey(butC,eKeyC);

  QRadioButton *butD = new QRadioButton(this);
  butD->move(300+marginx,160+marginy);
  butD->setCheckable(false);
  registerKey(butD,eKeyD);

  QRadioButton *butUp = new QRadioButton(this);
  butUp->move(300+marginx,10+marginy);
  butUp->setCheckable(false);
  registerKey(butUp,eKeyUp);

  QRadioButton *butDown = new QRadioButton(this);
  butDown->move(300+marginx,96+marginy);
  butDown->setCheckable(false);
  registerKey(butDown,eKeyDown);

  resize(350+2*marginx,200+2*marginy);

  startTimer(100);
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

  static int repeat = 0;

  if( m_keyPressed!=eKeyNone )
  {
    repeat++;
    if( repeat>5 )
    {
      if( lcdscreen::keyPressed(m_keyPressed)!=eKeyNone )
        repeat=0;
    }
  }
  else
    repeat=0;
}

void RaspiLcdWidget::mouseDoubleClickEvent ( QMouseEvent * /*event*/ )
{
  QClipboard *clip = QApplication::clipboard();
  QPixmap hcop = QPixmap::grabWidget(this);
  hcop.save("xxx.png");
  if( clip ) clip->setPixmap(hcop/*QPixmap::fromImage(m_image).scaled(256,128)*/);
}

void RaspiLcdWidget::registerKey(QAbstractButton *butt, keyType key)
{
  m_butList.append(butt);
  m_keyList.append(key);
  connect(butt,SIGNAL(pressed()),this,SLOT(buttonPressed()));
  connect(butt,SIGNAL(released()),this,SLOT(buttonReleased()));
}

void RaspiLcdWidget::buttonPressed()
{
  QAbstractButton *src = (QAbstractButton*)sender();

  for( int i=0; i<m_butList.count(); i++ )
  {
    if( m_butList[i]==src )
    {
      m_keyPressed = m_keyList[i];
      lcdscreen::keyPressed(m_keyPressed);
      break;
    }
  }
}
void RaspiLcdWidget::buttonReleased()
{
  m_keyPressed = eKeyNone;
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

QColor m_pencol = Qt::black;
QColor m_fillcol = Qt::white;

QTextEdit *m_debug = NULL;

void Qt_printf(char *message)
{
  if( m_debug==NULL )
  {
    m_debug = new QTextEdit(0);
    m_debug->setFont(QFont("Courier",10));
    m_debug->setWordWrapMode(QTextOption::NoWrap);
    m_debug->setReadOnly(true);
    m_debug->setPlainText("");
    m_debug->setGeometry(40,40,600,400);
    m_debug->show();
  }

  QString msg = message;
  m_debug->append(msg.replace("\n",""));
}

void Qt_ClearScreen(void)
{
  QPainter p(&m_window->m_image);
  p.fillRect(0,0,128,64,m_fillcol);
}

void Qt_SetPenColor(uint8 c)
{
  m_pencol = c ? Qt::black : Qt::white;
}

void Qt_SetFillColor(int8 c)
{
  m_fillcol = c ? Qt::black : Qt::white;
}

void Qt_SetContrast(uint8 /*contrast*/)
{
}

void Qt_PutPixel(uint8 x,uint8 y,uint8 color)
{
  m_window->m_image.setPixel(x, y, color ? 1 : 0);
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

void Qt_SetBacklight(uint8 light)
{
  if( light )
    m_background = QColor(Qt::white);
  else
    m_background = QColor(Qt::lightGray);
  if( m_window )
  {
    m_window->m_image.setColor(0,m_background.rgb());
    m_window->repaint();
  }
}
