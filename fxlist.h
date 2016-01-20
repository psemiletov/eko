#ifndef FXLIST_H
#define FXLIST_H

#include <QHash>


#include "afx.h"

typedef AFx* (*t_fx_creator)();


//available fx
class CFxList: public QObject
{
public:

 QList < AFx *> list;

 QHash <QString, t_fx_creator> hash;

 
 CFxList();
 ~CFxList();
 
 AFx *find_by_name (const QString &fxname);
 QStringList names();
};



#endif // FXLIST_H
