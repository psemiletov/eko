 /**************************************************************************
 *   2007-2016 by Peter Semiletov                                          *
 *   tea@list.ru                                                           *

public domain
 
 **************************************************************************/


#ifndef FMAN_H
#define FMAN_H


#include <QObject>
#include <QStandardItemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QDir>
#include <QDrag>
#include <QPainter>


class CFMan: public QTreeView
{
  Q_OBJECT

public:

 
  int sort_mode;
  Qt::SortOrder sort_order;
  
  QStandardItemModel *mymodel;
  QList <QFileInfo> list;
  
  QDir dir;
    
  CFMan (QWidget *parent = 0);
  //~CFMan();

  void add_entry (const QFileInfo &fi);
  void append_dot_entry (const QString &fname);

  const QModelIndex index_from_name (const QString &name);
  const QModelIndex index_from_idx (int idx);
  int get_sel_index();
  
  void nav (const QString &path);

  QString get_sel_fname();
  QStringList get_sel_fnames();

public slots:

  void tv_activated (const QModelIndex &index);
  void refresh();
  void dir_up();
  void cb_fman_currentChanged (const QModelIndex &current, const QModelIndex &previous);

  void header_view_sortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

  
signals:

 void file_activated (const QString &path);
 void dir_changed (const QString &path);
 void current_file_changed (const QString &path, const QString &just_name);

protected:

  void mouseMoveEvent (QMouseEvent *event);
  void keyPressEvent (QKeyEvent *event);

  void drawRow (QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};


#endif
