#include "src/gui/core/Application.h"

#include "src/Tracey.h"
#include "src/gui/core/ActionManager.h"
#include "src/gui/core/Configuration.h"
#include "src/gui/core/HighlightingData.h"
#include "src/gui/core/TraceManager.h"
#include "src/gui/dialogs/AboutQtDialog.h"
#include "src/gui/dialogs/AboutTraceyDialog.h"
#include "src/gui/dialogs/HelpDialog.h"
#include "src/gui/dialogs/MessageDialog.h"
#include "src/gui/main/TraceArea.h"
#include "src/gui/tools/MarkWidget.h"
#include "src/gui/tools/HighlightingWidget.h"
#include "src/gui/tools/SearchWidget.h"
#include "src/gui/widgets/toolbox/ToolBox.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QSplitter>
#include <QTextCodec>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
Application::Application(QWidget* parent) :
  QMainWindow(parent),
  helpDialog(nullptr)
{
  setWindowIcon(QIcon(":/Application/img/Icon.png"));
  setMinimumSize(1100, 750);

  QRect geometry = Config().getMainWindowGeometry();

  if (geometry.topLeft() == QPoint(-6666, -6666))
  {
    QRect screenGeometry = QApplication::desktop()->availableGeometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;

    move(x, y);
  }
  else
  {
    setGeometry(geometry);
  }

  init();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
ActionManager* Application::getActionManager() const
{
  return actionManager;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
HighlightingData* Application::getHighlightingData()
{
  return highlightingData;
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::changeEvent(QEvent* event)
{
  if (event->type() == QEvent::WindowStateChange)
  {
    if (isMinimized() == true)
    {
      bool minimizeToTray = Config().getBoolProperty(SETTINGS_MINIMIZE_TO_TRAY);

      if (minimizeToTray)
      {
        hide();

        return;
      }
    }
  }

  QMainWindow::changeEvent(event);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::closeEvent(QCloseEvent* event)
{
  bool closeToTray = Config().getBoolProperty(SETTINGS_CLOSE_TO_TRAY);

  if (closeToTray)
  {
    event->ignore();

    hide();
  }
  else
  {
    exitApplication();
  }

  event->ignore();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::hideEvent(QHideEvent* event)
{
  actionManager->setTrayLabel(false);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::showEvent(QShowEvent* event)
{
  actionManager->setTrayLabel(true);
}

//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::init()
{
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

  highlightingData = new HighlightingData(this);

  traceArea = new TraceArea(highlightingData, this);

  SearchWidget* searchWidget = new SearchWidget(this);

  HighlightingWidget* highlightingWidget = new HighlightingWidget(this);

  markWidget = new MarkWidget(this);

  ToolBox* commandPanel = new ToolBox(this);
  commandPanel->setMinimumWidth(400);
  commandPanel->setMaximumWidth(600);
  commandPanel->addItem(searchWidget, tr("Search"));
  commandPanel->addItem(highlightingWidget, tr("Highlighting"));
  commandPanel->addItem(markWidget, tr("Marks"));

  splitter = new QSplitter(Qt::Horizontal, this);
  splitter->addWidget(commandPanel);
  splitter->addWidget(traceArea);
  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);
  splitter->restoreState(Config().getMainWindowSplitterState());

  QFrame* centralWidget = new QFrame(this);
  centralWidget->setObjectName("CentralWidget");

  QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
  mainLayout->setMargin(5);
  mainLayout->addWidget(splitter);

  setCentralWidget(centralWidget);

  TraceManager* traceManager = new TraceManager(traceArea, this);

  actionManager = new ActionManager(traceManager, this);

  connect(actionManager, &ActionManager::findRequested, searchWidget, &SearchWidget::find);
  connect(actionManager, &ActionManager::findNextRequested, searchWidget, &SearchWidget::findNext);
  connect(actionManager, &ActionManager::findPreviousRequested, searchWidget, &SearchWidget::findPrevious);
  connect(searchWidget, &SearchWidget::findNextRequested, traceManager, &TraceManager::processFindNext);
  connect(searchWidget, &SearchWidget::findPreviousRequested, traceManager, &TraceManager::processFindPrevious);
  connect(searchWidget, &SearchWidget::countRequested, traceManager, &TraceManager::processCount);
  connect(searchWidget, &SearchWidget::goToRequested, traceManager, &TraceManager::processGoTo);
  connect(highlightingWidget, &HighlightingWidget::findNextRequested, traceManager, &TraceManager::processFindNext);
  connect(highlightingWidget, &HighlightingWidget::findPreviousRequested, traceManager, &TraceManager::processFindPrevious);
  connect(traceManager, &TraceManager::marksChanged, markWidget, &MarkWidget::setMarks);
  connect(markWidget, &MarkWidget::markRequested, traceManager, &TraceManager::goToMark);

  traceManager->loadTraceConfig();
}


//===================================================================================================================================================================================================//
// PUBLIC SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::exitApplication()
{
  bool showDialog = Config().getBoolProperty(SETTINGS_DIALOG_EXIT_APPLICATION);
  bool ok = true;

  if (showDialog)
  {
    MessageDialog dialog(this, tr("Exit Tracey?"), MessageDialog::Warning, true);
    ok = dialog.exec();

    Config().setProperty(SETTINGS_DIALOG_EXIT_APPLICATION, QString::number(!dialog.getDontAskAgain()));
  }

  if (ok)
  {
    Config().saveMainWindowState(normalGeometry(), isMaximized(), splitter->saveState());

    QApplication::quit();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::goToNextMark()
{
  markWidget->goToNextMark();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::goToPreviousMark()
{
  markWidget->goToPreviousMark();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::Trigger)
  {
    setApplicationVisibility();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::setApplicationVisibility()
{
  if (isVisible() && !isMinimized())
  {
    hide();
  }
  else
  {
    showApplication();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::showAboutWindow()
{
  AboutTraceyDialog* aboutTraceyDialog = new AboutTraceyDialog(this);
  aboutTraceyDialog->exec();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::showAboutQtWindow()
{
  AboutQtDialog* aboutQtDialog = new AboutQtDialog(this);
  aboutQtDialog->exec();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::showApplication()
{
  show();

  setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);

  activateWindow();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Application::showHelpWindow()
{
  if (helpDialog == nullptr)
  {
    helpDialog = new HelpDialog;
    helpDialog->showMaximized();
  }
  else
  {

    helpDialog->show();
  }
}