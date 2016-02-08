#include <QHBoxLayout>
#include <QPushButton>


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
