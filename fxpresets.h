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

  QString presets_path;

  QMap <QString, QString> map;

  QComboBox *cmb_presets;

  

  CFxPresets (QWidget *parent = 0);

  void load_presets_file (const QString &fname);
  void save_presets_file (const QString &fname);
  
signals:

  void preset_changed (const QString &path);

public slots:

  void cmb_presets_currentIndexChanged (const QString &text);

  

};

#endif // FXPRESETS_H
