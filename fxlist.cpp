#include "fxlist.h"
#include "fxset.h"


CFxList::CFxList()
{
  /*list.append (new CFxSimpleAmp);
  list.append (new CFxSimpleOverdrive);
  list.append (new CFxDelay);
  list.append (new CFxSimpleFilter);
  list.append (new CFxVynil);
  list.append (new CFxMetaluga);
  list.append (new CFxJest);
  */
  hash[CFxSimpleAmp::get_name()] = CFxSimpleAmp::create_self;
  hash[CFxSimpleOverdrive::get_name()] = CFxSimpleOverdrive::create_self;
  hash[CFxDelay::get_name()] = CFxDelay::create_self;
  hash[CFxSimpleFilter::get_name()] = CFxSimpleFilter::create_self;
  
  hash[CFxVynil::get_name()] = CFxVynil::create_self;
  hash[CFxMetaluga::get_name()] = CFxMetaluga::create_self;
  hash[CFxJest::get_name()] = CFxJest::create_self;
  
}


CFxList::~CFxList()
{
  /*foreach (AFx *f, list) 
          {
           delete f; 
          }*/
}
 

AFx *CFxList::find_by_name (const QString &fxname)
{
  /*for (int i = 0; i < list.size(); i++)
      {
       if (list[i]->name == fxname) 
          return list[i];
      }    

  return 0;*/
  return hash[fxname]();
}



QStringList CFxList::names()
{
 /* QStringList l;
  foreach (AFx *f, list)
          l.append (f->name);
  return l;        */
  
  QStringList l = hash.keys();
  l.sort();
  return l;
}

