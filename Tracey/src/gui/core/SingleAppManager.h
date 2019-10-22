#pragma once

#include <QObject>
#include <QLocalServer>

class SingleAppManager : public QObject
{
  Q_OBJECT

public:
  static SingleAppManager& instance();

  bool canRun();

private:
  explicit SingleAppManager(QObject* parent = nullptr);
  void checkPing(QLocalSocket *socket);
  bool createServer();
  bool isAnotherAppRunning();

  static const QLatin1String ping;
  QLocalServer server;
  QString serverName;

signals:
  void anotherAppStarted() const;

};
