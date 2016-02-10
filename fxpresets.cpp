#include <QHBoxLayout>
#include <QPushButton>
#include <QMenu>

#include "fxpresets.h"
#include "utils.h"

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

  QMenu *menu = new QMenu;
  bt_menu->setMenu (menu);
  
  menu_add_item (this, menu, tr ("Bank new"), SLOT (bank_new_click()), "", "");
  menu_add_item (this, menu, tr ("Bank load"), SLOT (bank_load_click()), "", "");
  menu_add_item (this, menu, tr ("Bank save"), SLOT (bank_save_click()), "", "");
  menu_add_item (this, menu, tr ("Bank save as"), SLOT (bank_save_as_click()), "", "");

  menu_add_item (this, menu, tr ("Preset save"), SLOT (preset_save()), "", "");
  menu_add_item (this, menu, tr ("Preset save as"), SLOT (preset_save_as()), "", "");
  
}


void CFxPresets::load_presets_file (const QString &fname)
{
  cmb_presets->clear();
  map.clear();
  map = map_load_keyval (fname);
  
  cmb_presets->addItems (map.keys());
}


void CFxPresets::save_presets_file (const QString &fname)
{
  if (fname.startsWith (":"))
     return;
     
  QString s = map_keyval_to_string (map);
  qstring_save (fname, s);
}


void CFxPresets::cmb_presets_currentIndexChanged (const QString &text)
{
  emit preset_changed (map[text]);
}


void CFxPresets::bank_new_click()
{


}
 

void CFxPresets::bank_save_click()
{


}


void CFxPresets::bank_save_as_click()
{


}


void CFxPresets::bank_load_click()
{


}


void CFxPresets::update_presets()
{


}


void CFxPresets::preset_save_as()
{


}
 

void CFxPresets::preset_save()
{


}
