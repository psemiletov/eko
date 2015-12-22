#ifndef ENVELOPE_H
#define ENVELOPE_H


#include <QObject>
#include <QList>

#define ENVELOPE_SIDE 20


class CEnvelope;

class CEnvelopePoint: public QObject
{
Q_OBJECT

public:

  size_t position_frames; //frames
  float value;
  bool selected;
   
  bool operator <(const CEnvelopePoint &other) const {return position_frames < other.position_frames;}
};


class CEnvelope: public QObject
{
Q_OBJECT

public:

  QList <CEnvelopePoint*> points;
 
  void insert_wise (int x, int y, int height, size_t maximum = 0);
  void select_point (CEnvelopePoint *e);
  void point_move (CEnvelopePoint *p, int x, int y, int height);
  void clear();
  CEnvelopePoint* get_selected();
  CEnvelopePoint* find (int frame, int y, int height, int frames_per_section);

  ~CEnvelope();
};


#endif // ENVELOPE_H
