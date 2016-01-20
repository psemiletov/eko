#include <QStandardItem>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDebug>

#include "gui_utils.h"
#include "fxrack.h"


void CFxRack::tv_activated (const QModelIndex &index)
{
  emit fx_activated (index.data().toString());

  int i = index.row();
  if (i != -1)
     effects.at(i)->show_ui();
}

/*
void CFxRack::add_entry (AFx *f, bool checked)
{
  QStandardItem *item = new QStandardItem (f->name);
  item->setCheckable (true);

  if (checked)
     item->setCheckState (Qt::Checked);

  item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled |
                  Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable |
                  Qt::ItemIsDropEnabled);

  model->appendRow (item);
}
*/

void CFxRack::ins_entry (AFx *f)
{
  QStandardItem *item = new QStandardItem (f->name);
  item->setCheckable (true);
  item->setCheckState (Qt::Checked);

  item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled |
                  Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable |
                  Qt::ItemIsDropEnabled);

  AFx *tfx = f;//f->self_create();

  int i = get_sel_index();
  if (i == -1)
     {
      model->appendRow (item);
      effects.append (tfx);
     }
  else
      {
       model->insertRow (i, item);
       effects.insert (i, tfx);
      }
      
  tfx->show_ui();    
}


void CFxRack::bt_up_clicked()
{
  if (! tree_view->selectionModel()->hasSelection())
     return;

  QModelIndex index = tree_view->selectionModel()->currentIndex();

  int row = index.row();
  if (row == 0)
     return;

  QList<QStandardItem *> l = model->takeRow (row);

  int new_row = row - 1;

  model->insertRow (new_row, l[0]);

  effects.swap (row, new_row);
}


void CFxRack::bt_down_clicked()
{

  if (! tree_view->selectionModel()->hasSelection())
     return;

  QModelIndex index = tree_view->selectionModel()->currentIndex();

  int row = index.row();
  if (row == model->rowCount() - 1)
     return;

  QList<QStandardItem *> l = model->takeRow (row);

  int new_row = row + 1 ;

  model->insertRow (new_row, l[0]);

  effects.swap (row, new_row);
}


CFxRack::CFxRack (QObject *parent): QObject (parent)
{
  avail_fx = new CFxList;

  model = new QStandardItemModel (0, 1, parent);

  connect (model, SIGNAL(dataChanged (const QModelIndex &, const QModelIndex &)),
          this, SLOT(bypass_dataChanged (const QModelIndex &, const QModelIndex &)));


  tree_view = new CFxTreeView;
  tree_view->setHeaderHidden (true);

  tree_view->setRootIsDecorated (false);
  tree_view->setModel (model);

  tree_view->setDragEnabled (true);

  //tree_view->header()->setResizeMode (QHeaderView::ResizeToContents);
  //tree_view->header()->setStretchLastSection (false);

  tree_view->setSelectionMode (QAbstractItemView::ExtendedSelection);
  tree_view->setSelectionBehavior (QAbstractItemView::SelectRows);

  connect (tree_view, SIGNAL(activated(const QModelIndex &)), this, SLOT(tv_activated(const QModelIndex &)));


  inserts = new QWidget;

  QVBoxLayout *v_box = new QVBoxLayout;
  inserts->setLayout (v_box);

  QPushButton *bt_add = new QPushButton ("+");
  QPushButton *bt_del = new QPushButton ("-");

  QToolButton *bt_up = new QToolButton;
  QToolButton *bt_down = new QToolButton;
  bt_up->setArrowType (Qt::UpArrow);
  bt_down->setArrowType (Qt::DownArrow);

  connect (bt_up, SIGNAL(clicked()), this, SLOT(bt_up_clicked()));
  connect (bt_down, SIGNAL(clicked()), this, SLOT(bt_down_clicked()));

  connect (bt_add, SIGNAL(clicked()), this, SLOT(add_fx()));
  connect (bt_del, SIGNAL(clicked()), this, SLOT(del_fx()));

  QHBoxLayout *h_box = new QHBoxLayout;
  h_box->addWidget (bt_add);
  h_box->addWidget (bt_del);
  h_box->addWidget (bt_up);
  h_box->addWidget (bt_down);

  v_box->addWidget (tree_view);
  v_box->addLayout (h_box);
}


void CFxRack::add_fx()
{
  CTextListWindow w (tr ("Select"), tr ("Available effects"));

  w.list->addItems (avail_fx->names());

  int result = w.exec();

  if (result != QDialog::Accepted)
      return;

  AFx *f = avail_fx->find_by_name (w.list->currentItem()->text());

  ins_entry (f);

  print_fx_list();
}


void CFxRack::del_fx()
{
  int i = get_sel_index();
  if (i == -1)
     return;

  QList<QStandardItem *> l = model->takeRow (i);
  delete l[0];

  AFx *f = effects.takeAt (i);
  if (f)
     delete f;

  print_fx_list();
}


CFxRack::~CFxRack()
{
  foreach (AFx *f, effects)
         {
          f->wnd_ui->close();
          delete f;
         }
 
  delete avail_fx;
}


QString CFxRack::get_sel_fname()
{
  if (! tree_view->selectionModel()->hasSelection())
      return QString();

  QModelIndex index = tree_view->selectionModel()->currentIndex();
  return index.data().toString();
}


const QModelIndex CFxRack::index_from_idx (int idx)
{
  QStandardItem *item = model->item (idx);
  if (item)
     return model->indexFromItem (item);
  else
      return QModelIndex();
}


int CFxRack::get_sel_index()
{
  if (! tree_view->selectionModel()->hasSelection())
     return -1;

  QModelIndex index = tree_view->selectionModel()->currentIndex();
  return index.row();
}


void CFxTreeView::mouseMoveEvent (QMouseEvent *event)
{
 /* if (! (event->buttons() & Qt::LeftButton))
     return;

  QStringList l = fman->get_sel_fnames();
  if (l.size() < 1)
     return;

  QDrag *drag = new QDrag (this);
  QMimeData *mimeData = new QMimeData;

  QList <QUrl> list;

  foreach (QString fn, l)
           list.append (QUrl::fromLocalFile (fn));

  mimeData->setUrls (list);
  drag->setMimeData (mimeData);

  if (drag->exec (Qt::CopyAction |
                  Qt::MoveAction |
                  Qt::LinkAction) == Qt::MoveAction)
     fman->refresh();
    */
  event->accept();
}


void CFxRack::print_fx_list()
{
  foreach (AFx *f, effects)
          qDebug() << f->name;
}


void CFxRack::bypass_dataChanged (const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
  bool b = model->data (topLeft, Qt::CheckStateRole).toInt();

  //qDebug() << model->data (topLeft, Qt::DisplayRole).toString() << " = " << b;
  //qDebug() << "row = " << topLeft.row();

  effects[topLeft.row()]->bypass = ! b;

  //if (effects[topLeft.row()]->bypass)
    // qDebug() << "bypassed";
}


void CFxRack::bypass_all (bool mode)
{
  for (int row = 0; row < model->rowCount(); row++)
      {
       QStandardItem *si = model->item (row);
       if (mode)
          si->setCheckState (Qt::Unchecked);
       else
          si->setCheckState (Qt::Checked);
      }
}


void CFxRack::set_state_all (FxState state)
{
  foreach (AFx *f, effects)
          {
           f->set_state (state);
          }
}


void CFxRack::reset_all_fx (size_t srate, size_t ch)
{
  foreach (AFx *f, effects)
          {
           f->reset_params (srate, ch);
          }
}

/*
const QModelIndex CFxRack::index_from_name (const QString &name)
{
  QList <QStandardItem *> lst = model->findItems (name);
  if (lst.size() > 0)
     return model->indexFromItem (lst[0]);
  else
      return QModelIndex();
}
*/
