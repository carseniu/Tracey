#include "src/gui/core/ActionManager.h"

#include "src/Tracey.h"
#include "src/gui/core/Application.h"
#include "src/gui/core/Configuration.h"
#include "src/gui/core/ToolBarStyle.h"
#include "src/gui/core/TraceManager.h"
#include "src/gui/main/TraceWidget.h"
#include "src/gui/widgets/traceviewer/TraceViewer.h"

#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QMenuBar>
#include <QToolBar>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
ActionManager::ActionManager(TraceManager* traceManager, Application* application) :
  application(application),
  traceManager(traceManager)
{
  createActions();
  createToolBar();
  createTrayIcon();
  createConnections();

  connect(traceManager, &TraceManager::autoscrollChanged, this, &ActionManager::processAutocrollChanged);
  connect(traceManager, &TraceManager::traceWidgetChanged, this, &ActionManager::processTraceWidgetChanged);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QList<QAction*> ActionManager::getActions()
{
  return actionList;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ActionManager::setShortcuts(const QStringList& shortcutList)
{
  for (int i = 0; i < actionList.size(); ++i)
  {
    setShortcut(actionList.at(i), shortcutList.at(i));
  }

  traceManager->updateShortcuts();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ActionManager::setTrayLabel(bool visible)
{
  if (visible)
  {
    showHideAction->setText(tr("Hide"));
  }
  else
  {
    showHideAction->setText(tr("Show"));
  }
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ActionManager::createActions()
{
  addTraceAction = new QAction(QIcon(":/Application/img/TraceAdd.png"), tr("Add Trace..."), this);
  setShortcut(addTraceAction, Config().getShortcut(SHORTCUT_ADD_TRACE));
  actionList.append(addTraceAction);
  
  addFilterAction = new QAction(QIcon(":/Application/img/FilterAdd.png"), tr("Add Filter..."), this);
  setShortcut(addFilterAction, Config().getShortcut(SHORTCUT_ADD_FILTER));
  addFilterAction->setEnabled(false);
  actionList.append(addFilterAction);

  clearAction = new QAction(QIcon(":/Application/img/Clear.png"), tr("Clear"), this);
  setShortcut(clearAction, Config().getShortcut(SHORTCUT_CLEAR));
  clearAction->setEnabled(false);
  actionList.append(clearAction);

  saveAction = new QAction(QIcon(":/Application/img/Save.png"), tr("Save"), this);
  setShortcut(saveAction, Config().getShortcut(SHORTCUT_SAVE));
  saveAction->setEnabled(false);
  actionList.append(saveAction);

  findAction = new QAction(QIcon(":/Application/img/Find.png"), tr("Find..."), this);
  setShortcut(findAction, Config().getShortcut(SHORTCUT_FIND));
  findAction->setEnabled(false);
  actionList.append(findAction);

  findNextAction = new QAction(QIcon(":/Application/img/FindNext.png"), tr("Find Next"), this);
  setShortcut(findNextAction, Config().getShortcut(SHORTCUT_FIND_NEXT));
  findNextAction->setEnabled(false);
  actionList.append(findNextAction);

  findPreviousAction = new QAction(QIcon(":/Application/img/FindPrevious.png"), tr("Find Previous"), this);
  setShortcut(findPreviousAction, Config().getShortcut(SHORTCUT_FIND_PREVIOUS));
  findPreviousAction->setEnabled(false);
  actionList.append(findPreviousAction);

  toggleMarkAction = new QAction(QIcon(":/Application/img/MarkToggle.png"), tr("Toggle Mark"), this);
  setShortcut(toggleMarkAction, Config().getShortcut(SHORTCUT_TOGGLE_MARK));
  toggleMarkAction->setEnabled(false);
  actionList.append(toggleMarkAction);

  nextMarkAction = new QAction(QIcon(":/Application/img/MarkNext.png"), tr("Next Mark"), this);
  setShortcut(nextMarkAction, Config().getShortcut(SHORTCUT_NEXT_MARK));
  nextMarkAction->setEnabled(false);
  actionList.append(nextMarkAction);

  previousMarkAction = new QAction(QIcon(":/Application/img/MarkPrevious.png"), tr("Previous Mark"), this);
  setShortcut(previousMarkAction, Config().getShortcut(SHORTCUT_PREVIOUS_MARK));
  previousMarkAction->setEnabled(false);
  actionList.append(previousMarkAction);

  deleteMarksAction = new QAction(QIcon(":/Application/img/MarkDelete.png"), tr("Delete Marks"), this);
  deleteMarksAction->setEnabled(false);

  settingsAction = new QAction(QIcon(":/Application/img/Settings.png"), tr("Settings..."), this);

  fontAction = new QAction(QIcon(":/Application/img/Font.png"), tr("Font..."), this);
  fontAction->setEnabled(false);

  QIcon autoscrollIcon;
  autoscrollIcon.addFile(":/Application/img/AutoscrollOff.png", QSize(), QIcon::Normal, QIcon::Off);
  autoscrollIcon.addFile(":/Application/img/AutoscrollOn.png", QSize(), QIcon::Normal, QIcon::On);
  autoscrollAction = new QAction(autoscrollIcon, tr("Autoscroll"), this);
  setShortcut(autoscrollAction, Config().getShortcut(SHORTCUT_AUTOSCROLL));
  autoscrollAction->setCheckable(true);
  autoscrollAction->setEnabled(false);
  actionList.append(autoscrollAction);


  // HELP
  helpAction = new QAction(QIcon(":/Application/img/Help.png"), tr("View Help"), this);
  setShortcut(helpAction, "F1");

  aboutTraceyAction = new QAction(QIcon(":/Application/img/AboutTracey.png"), tr("About Tracey"), this);

  aboutQtAction = new QAction(QIcon(":/Application/img/AboutQt.png"), tr("About Qt"), this);


  // TRAY
  showHideAction = new QAction(tr("Hide"), this);


  // EXTRA
  deleteTraceAction = new QAction(QIcon(":/Application/img/TraceDelete.png"), tr("Delete Trace"), this);
  setShortcut(deleteTraceAction, Config().getShortcut(SHORTCUT_DELETE_TRACE));
  actionList.append(deleteTraceAction);
  application->addAction(deleteTraceAction);

  deleteFilterAction = new QAction(QIcon(":/Application/img/FilterDelete.png"), tr("Delete Filter"), this);
  setShortcut(deleteFilterAction, Config().getShortcut(SHORTCUT_DELETE_FILTER));
  actionList.append(deleteFilterAction);
  application->addAction(deleteFilterAction);

  startStopAction = new QAction(QIcon(":/Application/img/Start.png"), tr("Start/Stop"), this);
  setShortcut(startStopAction, Config().getShortcut(SHORTCUT_START_STOP));
  actionList.append(startStopAction);
  application->addAction(startStopAction);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ActionManager::createConnections()
{
  connect(addTraceAction, &QAction::triggered, traceManager, &TraceManager::addTrace);
  connect(addFilterAction, &QAction::triggered, traceManager, &TraceManager::addFilter);
  connect(clearAction, &QAction::triggered, traceManager, &TraceManager::deleteAll);
  connect(saveAction, &QAction::triggered, traceManager, &TraceManager::save);
  connect(findAction, &QAction::triggered, this, &ActionManager::findRequested);
  connect(findNextAction, &QAction::triggered, this, &ActionManager::findNextRequested);
  connect(findPreviousAction, &QAction::triggered, this, &ActionManager::findPreviousRequested);
  connect(toggleMarkAction, &QAction::triggered, traceManager, &TraceManager::toggleMark);
  connect(nextMarkAction, &QAction::triggered, application, &Application::goToNextMark);
  connect(previousMarkAction, &QAction::triggered, application, &Application::goToPreviousMark);
  connect(deleteMarksAction, &QAction::triggered, traceManager, &TraceManager::deleteMarks);
  connect(settingsAction, &QAction::triggered, traceManager, &TraceManager::showSettingsDialog);
  connect(fontAction, &QAction::triggered, traceManager, &TraceManager::showFontDialog);
  connect(autoscrollAction, &QAction::toggled, traceManager, &TraceManager::setAutoscroll);

  // HELP
  connect(helpAction, &QAction::triggered, application, &Application::showHelpWindow);
  connect(aboutTraceyAction, &QAction::triggered, application, &Application::showAboutWindow);
  connect(aboutQtAction, &QAction::triggered, application, &Application::showAboutQtWindow);

  // TRAY
  connect(showHideAction, &QAction::triggered, application, &Application::setApplicationVisibility);

  // EXTRA
  connect(deleteTraceAction, &QAction::triggered, traceManager, &TraceManager::deleteTrace);
  connect(deleteFilterAction, &QAction::triggered, traceManager, &TraceManager::deleteFilter);
  connect(startStopAction, &QAction::triggered, traceManager, &TraceManager::toggleStartStop);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ActionManager::createToolBar()
{
  QToolBar* toolBar = application->addToolBar("");
  toolBar->setIconSize(QSize(40, 40));
  toolBar->setMovable(false);

  toolBar->addAction(addTraceAction);
  toolBar->addAction(addFilterAction);
  toolBar->addAction(clearAction);
  toolBar->addAction(saveAction);
  toolBar->addSeparator();
  toolBar->addAction(findAction);
  toolBar->addAction(findNextAction);
  toolBar->addAction(findPreviousAction);
  toolBar->addSeparator();
  toolBar->addAction(toggleMarkAction);
  toolBar->addAction(nextMarkAction);
  toolBar->addAction(previousMarkAction);
  toolBar->addAction(deleteMarksAction);
  toolBar->addSeparator();
  toolBar->addAction(settingsAction);
  toolBar->addAction(fontAction);
  toolBar->addSeparator();
  toolBar->addAction(autoscrollAction);
  
  QWidget* stretchWidget = new QWidget;
  stretchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  toolBar->addWidget(stretchWidget);

  toolBar->addAction(helpAction);
  toolBar->addAction(aboutTraceyAction);
  toolBar->addAction(aboutQtAction);

  ToolBarStyle* toolBarStyle = new ToolBarStyle;
  toolBar->setStyle(toolBarStyle);
  toolBar->widgetForAction(addTraceAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(addFilterAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(clearAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(saveAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(findAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(findPreviousAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(findNextAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(toggleMarkAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(previousMarkAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(nextMarkAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(deleteMarksAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(settingsAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(fontAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(autoscrollAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(helpAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(aboutTraceyAction)->setStyle(toolBarStyle);
  toolBar->widgetForAction(aboutQtAction)->setStyle(toolBarStyle);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ActionManager::createTrayIcon()
{
  QAction* exitAction = new QAction(QIcon(":/Application/img/Exit.png"), tr("Exit"), this);
  connect(exitAction, &QAction::triggered, application, &Application::exitApplication);

  QMenu* trayIconMenu = new QMenu(application);
  trayIconMenu->addAction(showHideAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(exitAction);

  QSystemTrayIcon* trayIcon = new QSystemTrayIcon(application);
  trayIcon->setToolTip("Tracey");
  trayIcon->setIcon(QIcon(":/Application/img/Icon.png"));
  trayIcon->setContextMenu(trayIconMenu);
  trayIcon->show();
  connect(trayIcon, &QSystemTrayIcon::activated, application, &Application::onTrayIconActivated);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ActionManager::setShortcut(QAction* action, const QString& shortcut)
{
  action->setShortcut(QKeySequence(shortcut));
  action->setToolTip(action->text() + " (" + shortcut + ")");
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ActionManager::processAutocrollChanged(bool autoscroll)
{
  QObject::disconnect(autoscrollAction, &QAction::toggled, traceManager, &TraceManager::setAutoscroll);
  autoscrollAction->setChecked(autoscroll);
  connect(autoscrollAction, &QAction::toggled, traceManager, &TraceManager::setAutoscroll);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ActionManager::processTraceWidgetChanged(TraceWidget* traceWidget)
{
  addFilterAction->setEnabled(false);
  clearAction->setEnabled(false);
  saveAction->setEnabled(false);
  findAction->setEnabled(false);
  findNextAction->setEnabled(false);
  findPreviousAction->setEnabled(false);
  toggleMarkAction->setEnabled(false);
  nextMarkAction->setEnabled(false);
  previousMarkAction->setEnabled(false);
  deleteMarksAction->setEnabled(false);
  fontAction->setEnabled(false);
  autoscrollAction->setEnabled(false);
  QObject::disconnect(autoscrollAction, &QAction::toggled, traceManager, &TraceManager::setAutoscroll);
  autoscrollAction->setChecked(false);
  connect(autoscrollAction, &QAction::toggled, traceManager, &TraceManager::setAutoscroll);

  if (traceWidget != nullptr)
  {
    addFilterAction->setEnabled(true);
    clearAction->setEnabled(true);
    saveAction->setEnabled(true);
    findAction->setEnabled(true);
    findNextAction->setEnabled(true);
    findPreviousAction->setEnabled(true);
    toggleMarkAction->setEnabled(true);
    nextMarkAction->setEnabled(true);
    previousMarkAction->setEnabled(true);
    deleteMarksAction->setEnabled(true);
    fontAction->setEnabled(true);
    autoscrollAction->setEnabled(true);


    QObject::disconnect(autoscrollAction, &QAction::toggled, traceManager, &TraceManager::setAutoscroll);
    autoscrollAction->setChecked(traceWidget->getTraceViewer()->isAutoscroll());
    connect(autoscrollAction, &QAction::toggled, traceManager, &TraceManager::setAutoscroll);
  }
}