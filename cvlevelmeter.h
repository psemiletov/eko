#ifndef CVLEVELMETER_H
#define CVLEVELMETER_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QResizeEvent>

class CVLevelMeter: public QWidget
{
  Q_OBJECT

public:
  
  bool init_state;

  QImage img_bar;
 
  int scale_width;
  int bars_width;
  
  float peak_l;
  float peak_r;
  float pl;
  float pr;

  CVLevelMeter (QWidget *parent);

  void update_scale_image();

protected:

 void paintEvent (QPaintEvent *event);
 void resizeEvent(QResizeEvent *event);
};

#endif
