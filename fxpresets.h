#ifndef FXPRESETS_H
#define FXPRESETS_H

#include <QObject>
#include <QMap>
#include <QWidget>
#include <QComboBox>


class CFxPresets: public QWidget
{
Q_OBJECT

public:

  QString preset_data;

  QString banks_path; //path to the plugin's banks
  
  QString path_bank; //full path of the current bank

  QMap <QString, QString> map;

  QComboBox *cmb_presets;

  

  CFxPresets (QWidget *parent = 0);

  void update_presets();

  void load_bank_file (const QString &fname);
  void save_bank_file (const QString &fname);
  
signals:

  void preset_changed (const QString &path);
  void save_request();
  

public slots:

  void cmb_presets_currentIndexChanged (const QString &text);

  void bank_new_click();
  void bank_load_click();
  void bank_save_click();
  void bank_save_as_click();
 
  void preset_save_as();
  void preset_save();

};

#endif // FXPRESETS_H
