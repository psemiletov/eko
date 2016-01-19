#include "fxlist.h"
#include "fxset.h"


CFxList::CFxList()
{
  list.append (new CFxSimpleAmp);
  list.append (new CFxSimpleOverdrive);
  list.append (new CFxDelay);
  list.append (new CFxSimpleFilter);
  list.append (new CFxVynil);
  list.append (new CFxMetaluga);
  list.append (new CFxJest);
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

