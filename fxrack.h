#ifndef FXRACK_H
#define FXRACK_H

#include <cstring>

#include <QWidget>
#include <QTreeView>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QStandardItemModel>

#include "afx.h"
#include "fxlist.h"


class CFxTreeView;
class CFxListInterface;


class CFxRack: public QObject
{
  Q_OBJECT

public:

  CFxList *avail_fx;

  QList <AFx*> effects;
  

  QWidget *inserts; //inserts section for the mixer, the "external" widget
  CFxTreeView *tree_view; //inside of the inserts widget

  QStandardItemModel *model;

  CFxRack (QObject *parent = 0);
  ~CFxRack();

  void add_entry (AFx *f, bool checked = true);
  void ins_entry (AFx *f);

  const QModelIndex index_from_name (const QString &name);
  const QModelIndex index_from_idx (int idx);
  int get_sel_index();

  void bypass_all (bool mode = true);
  void set_state_all (FxState state);
  void reset_all_fx (size_t srate, size_t ch);

  QString get_sel_fname();
  void print_fx_list();

public slots:

  void tv_activated (const QModelIndex &index);
  void add_fx();
  void del_fx();

  void bt_up_clicked();
  void bt_down_clicked();

  void bypass_dataChanged (const QModelIndex &topLeft, const QModelIndex &bottomRight);

signals:

 void fx_activated (const QString &path);
};


class CFxTreeView: public QTreeView
{
  Q_OBJECT

protected:

  void mouseMoveEvent (QMouseEvent *event);
};


#endif // FXRACK_H
