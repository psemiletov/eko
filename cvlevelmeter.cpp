#include <QPainter>
#include <QPoint>

#include "cvlevelmeter.h"
#include "utils.h"
#include "db.h"


void CVLevelMeter::update_scale_image()
{
  QImage im (scale_width, height(), QImage::Format_RGB32);

  QPainter painter (&im);

  painter.fillRect (0, 0, scale_width, height(), Qt::white);

  painter.setPen (Qt::black);
  painter.setFont (QFont ("Mono", 6));
 
  int ten = get_value (height(), 5);  
                    
  int percentage = 105;
  float ff = 0.0f;
  
  for (int y = 0; y < height(); y++)
      {
       if (! (y % ten))
          {
           percentage -= 5;
          
           ff = (1.0 / 100) * percentage;
                                            
           QPoint p1 (1, y);
           QPoint p2 (scale_width, y);
           painter.drawLine (p1, p2);
               
           painter.drawText (QPoint (1, y), QString::number (float2db (ff), 'g', 2)); //dB
          }
       }         
     
  img_bar = im;
}


void CVLevelMeter::resizeEvent(QResizeEvent *event)
{
  update_scale_image();
}


CVLevelMeter::CVLevelMeter (QWidget *parent)
{
  peak_l = 0;
  peak_r = 0;

  scale_width = 42;
  bars_width = 48;

  setMinimumWidth (scale_width + bars_width);

  resize (scale_width + bars_width, 256);
}


#define FALLOFF_COEF 0.05f

void CVLevelMeter::paintEvent (QPaintEvent *event)
{
  if (pl > peak_l)
     peak_l = pl;
  else   
      peak_l -= FALLOFF_COEF;
     
  if (pr > peak_r)
     peak_r = pr;
  else   
      peak_r -= FALLOFF_COEF;

  if (peak_l < -1 || peak_l > 1)
     peak_l = 0;

  if (peak_r < -1 || peak_r > 1)
     peak_r = 0;

  QPainter painter (this);   
   
  int h = height();
  int w = width();
  
  painter.fillRect (scale_width, 0, w, h, Qt::white);
       
  int lenl = h - (int)(peak_l * 1.0f * h);
  int lenr = h - (int)(peak_r * 1.0f * h);
      
  QPoint ltop (scale_width, h);
  QPoint lbottom (scale_width + (w - scale_width) / 2, lenl);
  
  QPoint rtop (scale_width + (w - scale_width) / 2, h);
  QPoint rbottom (w, lenr);
  
  QRect l (ltop, lbottom);
  QRect r (rtop, rbottom);
    
  painter.fillRect (l, Qt::green);
  painter.fillRect (r, Qt::darkGreen);

  painter.drawImage (1, 1, img_bar);
 
  event->accept();  
}
