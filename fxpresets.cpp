#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QDir>
#include <QDebug>

#include "fxpresets.h"
#include "utils.h"
#include "gui_utils.h"


CFxPresets::CFxPresets (QWidget *parent): QWidget (parent)
{
  
  QHBoxLayout *h_box = new QHBoxLayout;
  setLayout (h_box);
  
  cmb_presets = new QComboBox;
  QPushButton *bt_menu = new QPushButton (tr ("Menu"));

  h_box->addWidget (cmb_presets);
  h_box->addWidget (bt_menu);
  
  connect (cmb_presets, SIGNAL(currentIndexChanged (const QString &)),
           this, SLOT(cmb_presets_currentIndexChanged (const QString &)));


  menu_banks = new QMenu (tr ("Banks"));
 
  menu = new QMenu;
  bt_menu->setMenu (menu);
 
  menu->addMenu (menu_banks);
   
  menu_add_item (this, menu, tr ("Bank new"), SLOT (bank_new_click()), "", "");
  menu_add_item (this, menu, tr ("Bank load"), SLOT (bank_load_click()), "", "");
  menu_add_item (this, menu, tr ("Bank save"), SLOT (bank_save_click()), "", "");
  menu_add_item (this, menu, tr ("Bank save as"), SLOT (bank_save_as_click()), "", "");

  menu_add_item (this, menu, tr ("Preset save"), SLOT (preset_save()), "", "");
  menu_add_item (this, menu, tr ("Preset save as"), SLOT (preset_save_as()), "", "");
}


void CFxPresets::load_bank_file (const QString &fname)
{
  if (fname.isNull() || fname.isEmpty())
     return;

  cmb_presets->clear();
  map.clear();
  map = map_load_keyval (fname, "^I^S^");
  
  cmb_presets->addItems (map.keys());
}


void CFxPresets::save_bank_file (const QString &fname)
{
  if (fname.startsWith (":"))
     return;
     
//  qDebug() << "CFxPresets::save_bank_file "  << fname;
     
  QString s = map_keyval_to_string (map, "^I^S^");
  
  //qDebug() << s;
  
  qstring_save (fname, s);
}


void CFxPresets::cmb_presets_currentIndexChanged (const QString &text)
{
  emit preset_changed (map[text]);
}


void CFxPresets::bank_new_click()
{
  path_bank = "";
  map.clear();
  cmb_presets->clear();
}
 

void CFxPresets::bank_save_click()
{
  if (! file_exists (path_bank) || path_bank.indexOf (":") == 0)
     bank_save_as_click();
  else   
      save_bank_file (path_bank);
}


void CFxPresets::bank_save_as_click()
{
  create_bank_dir();

  QString f = QFileDialog::getSaveFileName (this, tr ("Save File"), banks_path);
  if (f.isNull())
     return;
  
  path_bank = f;
  save_bank_file (path_bank);
 
  menu_banks->clear();
  update_banks_list (banks_path);
}


void CFxPresets::bank_load_click()
{
  QString f = QFileDialog::getOpenFileName (this, tr ("Open File"), banks_path);  
  if (f.isNull())
     return;
  
  path_bank = f;
  load_bank_file (banks_path);
}


void CFxPresets::preset_save_as()
{
  bool ok;
  QString text = QInputDialog::getText(this, tr("Save preset as"),
                                         tr("Name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty())
        {
         emit save_request();
         map[text] = preset_data;
         cmb_presets->addItem (text);
        }
}
 

void CFxPresets::preset_save()
{
  if (cmb_presets->currentText().isNull())
     {
      preset_save_as();
      return;
     }
  
  emit save_request();
  map[cmb_presets->currentText()] = preset_data;
}


void CFxPresets::create_bank_dir()
{
  if (banks_path == "")
     return;
    
  QDir dr;
  dr.setPath (banks_path);
 
  if (! dr.exists())
     dr.mkpath (banks_path);
}    


void CFxPresets::bank_selected()
{
  QAction *a = qobject_cast<QAction *>(sender());
  QString fname (banks_path);
  fname.append ("/").append (a->text());
  
  load_bank_file (fname);
}


void CFxPresets::update_banks_list (const QString &path)
{
  banks_path = path;

  int i = banks_path.lastIndexOf ("/");
  fxname = banks_path.right (banks_path.length() - i - 1);
  
  qDebug() << "update_banks_list fxname:::::" << fxname; 

  QStringList l1 = read_dir_entries (banks_path);
  QStringList l2 = read_dir_entries (":/fxpresets" + fxname);

  l1 += l2;

  create_menu_from_list (this, menu_banks,
                         l1,
                         SLOT (bank_selected()));
}
