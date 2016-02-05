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
  modulenames[CFxSimpleAmp::get_modulename()] = CFxSimpleAmp::create_self;
  modulenames[CFxSimpleOverdrive::get_modulename()] = CFxSimpleOverdrive::create_self;
  modulenames[CFxDelay::get_modulename()] = CFxDelay::create_self;
  modulenames[CFxSimpleFilter::get_modulename()] = CFxSimpleFilter::create_self;
  modulenames[CFxVynil::get_modulename()] = CFxVynil::create_self;
  modulenames[CFxMetaluga::get_modulename()] = CFxMetaluga::create_self;
  modulenames[CFxJest::get_modulename()] = CFxJest::create_self;


  classnames["CFxSimpleAmp"] = CFxSimpleAmp::create_self;
  classnames["CFxSimpleOverdrive"] = CFxSimpleOverdrive::create_self;
  classnames["CFxDelay"] = CFxDelay::create_self;
  classnames["CFxSimpleFilter"] = CFxSimpleFilter::create_self;
  classnames["CFxVynil"] = CFxVynil::create_self;
  classnames["CFxMetaluga"] = CFxMetaluga::create_self;
  classnames["CFxJest"] = CFxJest::create_self;
  
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
  return modulenames[fxname]();
}



QStringList CFxList::names()
{
 /* QStringList l;
  foreach (AFx *f, list)
          l.append (f->name);
  return l;        */
  
  QStringList l = modulenames.keys();
  l.sort();
  return l;
}

