#ifndef FXLIST_H
#define FXLIST_H

#include "afx.h"

//available fx
class CFxList: public QObject
{
public:

 QList < AFx *> list;

 CFxList();
 ~CFxList();
 
 AFx *find_by_name (const QString &fxname);
 QStringList names();
};



#endif // FXLIST_H
