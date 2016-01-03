#include "fxlist.h"
#include "fxset.h"


CFxList::CFxList()
{
  list.append (new CFxSimpleAmp (1));
  list.append (new CFxSimpleOverdrive (1));
  list.append (new CFxPitchShift (1));
  list.append (new CFxSimpleFilter (1));
  list.append (new CFxClassicOverdrive (1));
  
  
}


CFxList::~CFxList()
{
  foreach (AFx *f, list) 
          {
           delete f; 
          }
}
 

AFx *CFxList::find_by_name (const QString &fxname)
{
  for (int i = 0; i < list.size(); i++)
      {
       if (list[i]->name == fxname) 
          return list[i];
      }    

  return 0;
}



QStringList CFxList::names()
{
  QStringList l;
  foreach (AFx *f, list)
          l.append (f->name);
  return l;        
}

