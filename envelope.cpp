#include <QRect>
#include <QDebug>

#include <algorithm>

#include "utils.h"
#include "envelope.h"


bool comp_ep (CEnvelopePoint *e1, CEnvelopePoint *e2)
{
  return e1->position_frames < e2->position_frames;
}


CEnvelopePoint* CEnvelope::find (int frame, int y, int height, int frames_per_section)
{
  foreach (CEnvelopePoint *t, points)
          {
           int t_y = get_fvalue (height, t->value);
           int t_x = t->position_frames;
           
           QRect rectangle (t_x, t_y, frames_per_section * ENVELOPE_SIDE, ENVELOPE_SIDE);
           
           if (rectangle.contains (frame, y))
               return t; 
          }
          
  return 0;        
}


CEnvelopePoint* CEnvelope::get_selected()
{
  CEnvelopePoint *p = 0;
  
  for (int i = 0; i < points.size(); i++)
      {
       if (points[i]->selected)
          {
           p = points[i];
           break;
          }
      } 
       
  return p;
}


void CEnvelope::clear()
{
  for (int i = 0; i < points.size(); i++)
       delete points[i];
      
  points.clear();    
}


void CEnvelope::select_point (CEnvelopePoint *e)
{
  foreach (CEnvelopePoint *t, points)
          {
           t->selected = false;
          }
          
   e->selected = true;       
}


CEnvelope::~CEnvelope()
{
  for (int i = 0; i < points.size(); i++)
     delete points[i];
}


void CEnvelope::insert_wise (int x, int y, int height, size_t maximum)
{
  CEnvelopePoint *e = new CEnvelopePoint;
  
  e->position_frames = x;
  
  if (points.size() == 0)
     e->value = 50;   
  else
      e->value = get_percent (height, (float)y);
  
  if (points.size() == 0)
     {
      CEnvelopePoint *point_start = new CEnvelopePoint;
  
      point_start->position_frames = 0;
      point_start->value = 50.0f;
    
      CEnvelopePoint *point_end = new CEnvelopePoint;
  
      point_end->position_frames = maximum;
      point_end->value = 50.0f;
    
      points.append (point_start);
      points.append (e);
      points.append (point_end);

      std::stable_sort  (points.begin(), points.end(), comp_ep);
 
      return;
     }

  //is there duplicated position? if yes, replace it with a new item
  for (int i = 0; i < points.size(); i++)
      {
       if (e->position_frames == points[i]->position_frames)
          {
           delete points[i];
           points.removeAt (i);
           break;
          }
      } 
       
  points << e;
 
  std::stable_sort (points.begin(), points.end(), comp_ep);

  int idx = points.indexOf (e);
     
  if (idx == 1)
      points[0]->value = e->value;

  if (idx == points.size() - 2)
      points[points.size()-1]->value = e->value;
}


void CEnvelope::point_move (CEnvelopePoint *p, int x, int y, int height)
{
  if (! p)
     return;

  int idx = points.indexOf (p);

  if ((idx != points.size() - 1) && (idx != 0))
     {
      p->position_frames = x;
      p->value = get_percent (height, (float)y);
     }
  else
      p->value = get_percent (height, (float)y);

          
  //if (idx == 1)
    //  points[0]->value = p->value;


  if (idx == points.size() - 2)
      points[points.size() - 1]->value = p->value;

  std::stable_sort (points.begin(), points.end(), comp_ep);
}
