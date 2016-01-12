/***************************************************************************
 *   2007-2016 by Peter Semiletov                            *
 *   tea@list.ru                                             *
 
 this code is Public Domain
 
 ***************************************************************************/

#ifndef SHORTCUTS_H
#define SHORTCUTS_H


#include <QObject>
#include <QHash>
#include <QLineEdit>


class CShortcutEntry: public QLineEdit
{
  Q_OBJECT

public:

  void keyPressEvent (QKeyEvent *event);
};


class CShortcuts: public QObject
{
  Q_OBJECT

public:

  QWidget *w;
  QString fname;
  QHash <QString, QString> hash;
  QStringList captions;

  CShortcuts (QWidget *widget);
  void captions_iterate();
  QAction* find_by_caption (const QString &text);
  QAction* find_by_shortcut (const QString &shcut);

  QKeySequence find_seq_by_caption (const QString &text);
  void set_new_shortcut (const QString &menuitem, const QString &shcut);

  void save_to_file (const QString &file_name);
  void load_from_file (const QString &file_name);
};

#endif
