#pragma once

#include <QObject>

#include <QSystemTrayIcon>

class Application;
class MenuStyle;
class TraceWidget;
class TraceManager;

class QAction;


class ActionManager : public QObject
{
  Q_OBJECT

public:
  ActionManager(TraceManager* traceManager, Application* application);

  QList<QAction*> getActions();
  void setShortcuts(const QStringList& shortcutList);
  void setTrayLabel(bool visible);

private:
  void createActions();
  void createConnections();
  void createToolBar();
  void createTrayIcon();
  void setShortcut(QAction* action, const QString& shortcut);

  Application* application;
  TraceManager* traceManager;

  QAction* addTraceAction;
  QAction* addFilterAction;
  QAction* clearAction;
  QAction* saveAction;
  QAction* findAction;
  QAction* findNextAction;
  QAction* findPreviousAction;
  QAction* toggleMarkAction;
  QAction* nextMarkAction;
  QAction* previousMarkAction;
  QAction* deleteMarksAction;
  QAction* settingsAction;
  QAction* fontAction;
  QAction* autoscrollAction;

  // HELP
  QAction* helpAction;
  QAction* aboutQtAction;
  QAction* aboutTraceyAction;

  // TRAY
  QAction* showHideAction;

  // EXTRA
  QAction* deleteTraceAction;
  QAction* deleteFilterAction;
  QAction* startStopAction;

  QList<QAction*> actionList;

private slots:
  void processAutocrollChanged(bool autoscroll);
  void processTraceWidgetChanged(TraceWidget* traceWidget);

signals:
  void findRequested();
  void findNextRequested();
  void findPreviousRequested();

};