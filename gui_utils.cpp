/*
this code is Public Domain
*/


#include "gui_utils.h"
#include "utils.h"

#include <QDir>
#include <QLabel>
#include <QPushButton>


void create_menu_from_list (QObject *handler,
                            QMenu *menu,
                            const QStringList &list,
                            const char *method
                           )
{
  menu->setTearOffEnabled (true);

  for (const auto &s:  list)
      {
       if (! s.startsWith("#"))
         {
          QAction *act = new QAction (s, menu->parentWidget());
          act->setData (s);
          handler->connect (act, SIGNAL(triggered()), handler, method);
          menu->addAction (act);
         }
     }
}


void create_menu_from_dir (QObject *handler,
                           QMenu *menu,
                           const QString &dir,
                           const char *method
                           )
{
  menu->setTearOffEnabled (true);
  QDir d (dir);
  QFileInfoList lst_fi = d.entryInfoList (QDir::NoDotAndDotDot | QDir::AllEntries,
                                          QDir::DirsFirst | QDir::IgnoreCase |
                                          QDir::LocaleAware | QDir::Name);

  for (const auto &fi: lst_fi)

  //foreach (QFileInfo fi, lst_fi)
         {
          if (fi.isDir())
             {
              QMenu *mni_temp = menu->addMenu (fi.fileName());
              create_menu_from_dir (handler, mni_temp,
                                    fi.filePath(), method);
             }
          else
              {
               QAction *act = new QAction (fi.fileName(), menu->parentWidget());
               act->setData (fi.filePath());
               handler->connect (act, SIGNAL(triggered()), handler, method);
               menu->addAction (act);
              }
         }
}

//uses dir name as menuitem, no recursion
void create_menu_from_dir_dir (QObject *handler,
                               QMenu *menu,
                               const QString &dir,
                               const char *method
                               )
{
  menu->setTearOffEnabled (true);
  QDir d (dir);
  QFileInfoList lst_fi = d.entryInfoList (QDir::NoDotAndDotDot | QDir::Dirs,
                                          QDir::IgnoreCase | QDir::LocaleAware | QDir::Name);

  for (const auto &fi: lst_fi)
         {
          if (fi.isDir())
             {
              QAction *act = new QAction (fi.fileName(), menu->parentWidget());
              act->setData (fi.filePath());
              handler->connect (act, SIGNAL(triggered()), handler, method);
              menu->addAction (act);
             }
         }
}

QLineEdit* new_line_edit (QBoxLayout *layout, const QString &label, const QString &def_value)
{
  QHBoxLayout *lt_h = new QHBoxLayout;
  QLabel *l = new QLabel (label);

  QLineEdit *r = new QLineEdit;
  r->setText (def_value);

  lt_h->addWidget (l);
  lt_h ->addWidget (r);

  layout->addLayout (lt_h);

  return r;
}


QSpinBox* new_spin_box (QBoxLayout *layout, const QString &label, int min, int max, int value, int step)
{
  QHBoxLayout *lt_h = new QHBoxLayout;
  QLabel *l = new QLabel (label);

  QSpinBox *r = new QSpinBox;

  r->setSingleStep (step);
  r->setRange (min, max);
  r->setValue (value);

  lt_h->addWidget (l);
  lt_h ->addWidget (r);

  layout->addLayout (lt_h);

  return r;
}


QComboBox* new_combobox (QBoxLayout *layout,
                         const QString &label,
                         const QStringList &items,
                         const QString &def_value)
{
  QHBoxLayout *lt_h = new QHBoxLayout;
  QLabel *l = new QLabel (label);

  QComboBox *r = new QComboBox;

  r->addItems (items);
  r->setCurrentIndex (r->findText (def_value));

  lt_h->addWidget (l);
  lt_h->addWidget (r);

  layout->addLayout (lt_h);

  return r;
}


CTextListWindow::CTextListWindow (const QString &title, const QString &label_text): QDialog (0) 
{
  //setAttribute (Qt::WA_DeleteOnClose);
  QVBoxLayout *lt = new QVBoxLayout;

  QLabel *l = new QLabel (label_text);

  list = new QListWidget (this);
   
  lt->addWidget (l);
  lt->addWidget (list);
    
  QHBoxLayout *lt_h = new QHBoxLayout;
  
  QPushButton *bt_apply = new QPushButton (tr ("OK"));
  QPushButton *bt_exit = new QPushButton (tr ("Exit"));

  connect (list, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(accept()));


  connect (bt_apply, SIGNAL(clicked()), this, SLOT(accept()));
  connect (bt_exit, SIGNAL(clicked()), this, SLOT(reject()));

  lt_h->addWidget (bt_apply);
  lt_h->addWidget (bt_exit);

  lt->addLayout (lt_h);

  setLayout (lt);
    
  setWindowTitle (title);
}


double input_double_value (const QString &caption, const QString &lbl,
                           double minval, double maxval, double defval, double step)
{
  double result = 0;

  QDialog wnd;

  wnd.setWindowTitle (caption);

  QVBoxLayout *v_box = new QVBoxLayout;
  wnd.setLayout (v_box);

  QLabel *label = new QLabel (lbl);

  QLineEdit ed;

  v_box->addWidget (&ed);
  v_box->addWidget (label);

  QHBoxLayout *lt_h = new QHBoxLayout;

  QPushButton *bt_apply = new QPushButton ("OK");
  QPushButton *bt_exit = new QPushButton ("Cancel");

  wnd.connect (bt_exit, SIGNAL(clicked()), &wnd, SLOT(reject()));
  wnd.connect (bt_apply, SIGNAL(clicked()), &wnd, SLOT(accept()));

  lt_h->addWidget (bt_apply);
  lt_h->addWidget (bt_exit);

  v_box->addLayout (lt_h);

  if (wnd.exec())
     //result = spb_d->value();
     result = ed.text().toDouble();

  return result;
}


QAction* menu_add_item (QObject *obj,
                      QMenu *menu,
                      const QString &caption,
                      const char *method,
                      const QString &shortkt,
                      const QString &iconpath
                     )
{
  QAction *act = new QAction (caption, obj);

  if (! shortkt.isEmpty())
     act->setShortcut (shortkt);

  if (! iconpath.isEmpty())
     act->setIcon (QIcon (iconpath));

  obj->connect (act, SIGNAL(triggered()), obj, method);
  menu->addAction (act);
  return act;
}
