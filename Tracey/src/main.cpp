#include "src/gui/core/Application.h"

#include "src/gui/core/SingleAppManager.h"
#include "src/gui/core/Configuration.h"

#include <QApplication>
#include <QFile>
#include <QtPlugin>


int main(int argc, char *argv[])
{
  Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
  Q_IMPORT_PLUGIN(QSQLiteDriverPlugin);
  Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin)
  Q_IMPORT_PLUGIN(QGifPlugin)

  qSetMessagePattern("%{file}(%{line}): %{message}");

  QApplication a(argc, argv);
  QApplication::setFont(QFont("Segoe UI", 8));

  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  Application application;
  QObject::connect(&SingleAppManager::instance(), &SingleAppManager::anotherAppStarted, &application, &Application::showApplication);

  if (!SingleAppManager::instance().canRun())
  {
    return 1;
  }

  qRegisterMetaType<QVector<qint64>>("QVector<qint64>");

  QFile styleFile(":/Application/qss/Style.qss");
  styleFile.open(QFile::ReadOnly);
  QString style(styleFile.readAll());

  QString backgroundLight = "#f8f8f8";
  QString backgroundDark = "#d8dddf"; 
  QString borderLight = "#b8babd";
  QString borderDark = "#727a7e";

  a.setStyleSheet(style.arg(backgroundLight, backgroundDark, borderLight, borderDark));

  if (Config().isMainWindowMaximized())
  {
    application.showMaximized();
  }
  else
  {
    application.show();
  }

  return a.exec();
}
