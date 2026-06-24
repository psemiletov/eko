#include <QApplication>
#include "eko.h"
#include "single_application_shared.h"

int main (int argc, char *argv[])
{
  Q_INIT_RESOURCE (eko);

  CSingleApplicationShared app (argc, argv, "eko_unique_key");
  qApp->setApplicationName ("EKO");

  // Если уже есть запущенный экземпляр - отправляем ему аргументы
  if (app.alreadyExists())
  {
    QStringList args = app.arguments();
    if (args.size() > 1)
    {
      for (int i = 1; i < args.size(); i++)
      {
        app.sendMessage(args.at(i));
      }
    }
    return 0; // Завершаем этот экземпляр
  }

  // Это первый (master) экземпляр
  CEKO mw;

  // Подключаем сигнал для получения аргументов от других экземпляров
  QObject::connect(&app, &CSingleApplicationShared::messageAvailable,
                   &mw, &CEKO::slot_open_files_from_args);

  // Обрабатываем аргументы командной строки для первого запуска
  mw.handle_args();

  mw.show();

  return app.exec();
}
