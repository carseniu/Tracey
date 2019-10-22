#include "src/gui/main/TraceArea.h"

#include "src/Tracey.h"
#include "src/gui/core/Application.h"
#include "src/gui/core/Configuration.h"
#include "src/gui/core/HighlightingData.h"
#include "src/gui/core/HistoryModel.h"
#include "src/gui/dialogs/InputDialog.h"
#include "src/gui/dialogs/MessageDialog.h"
#include "src/gui/main/SourcePanel.h"
#include "src/gui/main/SourceWidget.h"
#include "src/gui/main/TraceWindow.h"
#include "src/gui/main/TraceWindowTabWidget.h"
#include "src/logic//filter/FilterData.h"
#include "src/logic/source/SourceData.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QMenu>
#include <QTabBar>
#include <QTabWidget>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceArea::TraceArea(HighlightingData* highlightingData, Application* application) :
  QFrame(application), 
  highlightingData(highlightingData)
{
  sourceHistoryModel = new HistoryModel(HistoryModel::Source, this);

  traceWindowTabWidget = new TraceWindowTabWidget(this);
  traceWindowTabWidget->setTabBarBackground("#ffffff");

  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(traceWindowTabWidget);

  connect(traceWindowTabWidget, &TabWidget::currentIndexChanged, this, &TraceArea::onTraceWindowChanged);
  connect(traceWindowTabWidget, &TraceWindowTabWidget::fileDropped, this, &TraceArea::fileDropped);
  connect(traceWindowTabWidget, &TabWidget::contextMenuRequested, this, &TraceArea::showContextMenu);
  connect(traceWindowTabWidget, &TabWidget::tabMoved, this, &TraceArea::onTabMoved);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceWindow* TraceArea::addTraceWindow(SourceData* sourceData, const TraceInfo& traceInfo)
{
  TraceWindow* traceWindow = new TraceWindow(sourceData, highlightingData, sourceHistoryModel, traceInfo, this);
  traceWindowTabWidget->addTab(traceWindow, traceInfo.name);

  connect(traceWindow, &TraceWindow::autoscrollChanged, this, &TraceArea::autoscrollChanged);
  connect(traceWindow, &TraceWindow::deleteSourceHistoryRequested, this, &TraceArea::deleteSourceHistory);
  connect(traceWindow, &TraceWindow::nameChanged, this, &TraceArea::processNameChanged);
  connect(traceWindow, &TraceWindow::traceWidgetChanged, this, &TraceArea::traceWidgetChanged);

  emit traceWindowAdded(traceWindow);
  emit traceWidgetChanged(traceWindow->getCurrentTraceWidget());

  return traceWindow;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceWidget* TraceArea::getCurrentTraceWidget()
{
  TraceWidget* traceWidget = nullptr;
  TraceWindow* traceWindow = getCurrentTraceWindow();

  if (traceWindow != nullptr)
  {
    traceWidget = getCurrentTraceWindow()->getCurrentTraceWidget();
  }

  return traceWidget;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceWindow* TraceArea::getCurrentTraceWindow()
{
  return qobject_cast<TraceWindow*>(traceWindowTabWidget->getCurrentWidget());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int TraceArea::getNewTraceNumber(const QString& name) const
{
  return traceWindowTabWidget->getNewTabNumber(name);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int TraceArea::getPosition(TraceWindow* traceWindow) const
{
  return traceWindowTabWidget->getIndexOf(traceWindow);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::setCurentIndex(int index)
{
  traceWindowTabWidget->setCurrentIndex(index);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::setFont(const QFont& font)
{
  for (int i = 0; i < traceWindowTabWidget->getCount(); ++i)
  {
    TraceWindow* traceWindow = qobject_cast<TraceWindow*>(traceWindowTabWidget->getWidget(i));
    traceWindow->setFont(font);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::setHighlightingRules(const QVector<HighlightingRule*>& rules)
{
  for (int i = 0; i < traceWindowTabWidget->getCount(); ++i)
  {
    TraceWindow* traceWindow = qobject_cast<TraceWindow*>(traceWindowTabWidget->getWidget(i));
    traceWindow->setHighlightingRules(rules);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::updateShortcuts()
{
  for (int i = 0; i < traceWindowTabWidget->getCount(); ++i)
  {
    TraceWindow* traceWindow = qobject_cast<TraceWindow*>(traceWindowTabWidget->getWidget(i));
    traceWindow->updateShortcuts();
  }
}



//===================================================================================================================================================================================================//
// PUBIC SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::deleteTrace()
{
  TraceWindow* traceWindow = getCurrentTraceWindow();

  if (traceWindow != nullptr)
  {
    bool showDialog = Config().getBoolProperty(SETTINGS_DIALOG_DELETE_TRACE);
    bool ok = true;

    if (showDialog)
    {
      MessageDialog dialog(this, tr("Do you want to delete the trace <b><font color=#ff0000>") + traceWindow->getSourceData()->getName() + "</font></b>?", MessageDialog::Warning, true);
      ok = dialog.exec();

      Config().setProperty(SETTINGS_DIALOG_DELETE_TRACE, QString::number(!dialog.getDontAskAgain()));
    }

    if (ok)
    {
      SourceData* sourceData = traceWindow->getSourceData();
      QString traceId = sourceData->getId();

      delete sourceData;

      traceWindowTabWidget->removeTab(traceWindowTabWidget->getIndexOf(traceWindow));

      delete traceWindow;

      Config().deleteTrace(traceId);
    }



    if (traceWindowTabWidget->getCount() == 0)
    {
      emit traceWidgetChanged(nullptr);
    }
  }
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::cloneTrace()
{
  TraceWindow* traceWindow = getCurrentTraceWindow();
  TraceInfo traceInfo = traceWindow->getTraceInfo();

  InputDialog dialog(this, tr("Clone Trace"), tr("Name"), traceInfo.name + tr("_clone"));
  bool ok = dialog.exec();

  if (ok)
  {
    SourceData* sourceData = traceWindow->getSourceData();

    SourceData* newSourceData = new SourceData(createId(), dialog.getTextValue(), this);
    newSourceData->setSource(sourceData->getSource());

    traceInfo.name = dialog.getTextValue();
    TraceWindow* newTraceWindow = addTraceWindow(newSourceData, traceInfo);

    Config().addTrace(newTraceWindow->getTraceInfo());

    emit traceWindowAdded(newTraceWindow);

    for (FilterWidget* filterWidget : traceWindow->getFilterWidgetList())
    {
      FilterData* newFilterData = new FilterData(createId(), filterWidget->getFilterData()->getName(), newSourceData);

      FilterWidget* newFilterWidget = newTraceWindow->addFilterWidget(newFilterData, filterWidget->getFilterInfo());

      Config().addFilter(newFilterWidget->getFilterInfo());
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::deleteSourceHistory()
{
  bool showDialog = Config().getBoolProperty(SETTINGS_DIALOG_DELETE_SOURCE_HISTORY);
  bool ok = true;

  if (showDialog)
  {
    MessageDialog dialog(this, tr("Delete source history?"), MessageDialog::Warning, true);
    ok = dialog.exec();

    Config().setProperty(SETTINGS_DIALOG_DELETE_SOURCE_HISTORY, QString::number(!dialog.getDontAskAgain()));
  }

  if (ok)
  {
    sourceHistoryModel->setStringList(QStringList());
  }  
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::onTabMoved(int newIndex, int oldIndex)
{
  QVariantList ids; 
  QVariantList positions;

  for (int i = 0; i < traceWindowTabWidget->getCount(); ++i)
  {
    TraceWindow* traceWindow = qobject_cast<TraceWindow*>(traceWindowTabWidget->getWidget(i));
    TraceInfo traceInfo = traceWindow->getTraceInfo();

    ids << traceInfo.id;
    positions << i;
  }

  Config().updateTracePositions(ids, positions);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::onTraceWindowChanged(int index)
{
  Config().saveCurrentTraceWindowIndex(index);
  
  emit traceWidgetChanged(getCurrentTraceWidget());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::processNameChanged(TraceWindow* traceWindow, const QString& name)
{
  traceWindowTabWidget->setTabText(traceWindowTabWidget->getIndexOf(traceWindow), name);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::renameTrace()
{
  TraceWindow* traceWindow = getCurrentTraceWindow();
  TraceInfo traceInfo = traceWindow->getTraceInfo();

  InputDialog dialog(this, tr("Rename Trace"), tr("Name"), traceInfo.name);
  bool ok = dialog.exec();

  if (ok)
  {
    SourceData* sourceData = traceWindow->getSourceData();
    sourceData->setName(dialog.getTextValue());

    Config().updateTrace(traceWindow->getTraceInfo());
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceArea::showContextMenu(const QPoint& point)
{
  QAction* deleteTraceAction = new QAction(QIcon(":/Application/img/TraceDelete.png"), tr("Delete Trace"), this);
  connect(deleteTraceAction, &QAction::triggered, this, &TraceArea::deleteTrace);

  QAction* renameTraceAction = new QAction(QIcon(":/Application/img/TraceRename.png"), tr("Rename Trace..."), this);
  connect(renameTraceAction, &QAction::triggered, this, &TraceArea::renameTrace);

  QAction* cloneTraceAction = new QAction(QIcon(":/Application/img/TraceClone.png"), tr("Clone Trace..."), this);
  connect(cloneTraceAction, &QAction::triggered, this, &TraceArea::cloneTrace);

  QMenu menu(this);
  menu.addAction(deleteTraceAction);
  menu.addAction(renameTraceAction);
  menu.addAction(cloneTraceAction);

  menu.exec(point);
}