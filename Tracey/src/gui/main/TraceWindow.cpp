#include "TraceWindow.h"

#include "src/Tracey.h"
#include "src/gui/core/Configuration.h"
#include "src/gui/dialogs/InputDialog.h"
#include "src/gui/dialogs/MessageDialog.h"
#include "src/gui/main/FilterPanel.h"
#include "src/gui/main/FilterWidget.h"
#include "src/gui/main/SourcePanel.h"
#include "src/gui/main/SourceWidget.h"
#include "src/gui/main/TraceArea.h"
#include "src/gui/main/TraceTabWidget.h"
#include "src/logic/filter/FilterData.h"
#include "src/logic/source/SourceData.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QMenu>
#include <QTabWidget>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceWindow::TraceWindow(SourceData* sourceData, HighlightingData* highlightingData, HistoryModel* historyModel, const TraceInfo& traceInfo, TraceArea* traceArea) :
  QFrame(traceArea),
  traceArea(traceArea),
  highlightingData(highlightingData)
{
  traceWidgetTabWidget = new TraceTabWidget(this);

  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(traceWidgetTabWidget);

  sourceWidget = new SourceWidget(sourceData, highlightingData, historyModel, traceInfo, this);
  traceWidgetTabWidget->addTab(sourceWidget, tr("Source"), true);

  connect(traceWidgetTabWidget, &TabWidget::currentIndexChanged, this, &TraceWindow::onTraceWidgetChanged);
  connect(traceWidgetTabWidget, &TabWidget::tabMoved, this, &TraceWindow::onTabMoved);
  connect(traceWidgetTabWidget, &TabWidget::contextMenuRequested, this, &TraceWindow::showContextMenu);
  connect(sourceWidget, &SourceWidget::autoscrollChanged, this, &TraceWindow::autoscrollChanged);
  connect(sourceWidget, &SourceWidget::deleteSourceHistoryRequested, this, &TraceWindow::deleteSourceHistoryRequested);
  connect(sourceWidget, &SourceWidget::marksChanged, this, &TraceWindow::marksChanged);
  connect(sourceWidget, &SourceWidget::nameChanged, this, &TraceWindow::onNameChanged);
  connect(sourceWidget, &SourceWidget::traceWidgetChanged, this, &TraceWindow::traceWidgetChanged);
  connect(sourceWidget, &FilterWidget::deleteTraceWindowRequested, this, &TraceWindow::deleteTraceWindowRequested);

  onTraceWidgetChanged(0);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterWidget* TraceWindow::addFilterWidget(FilterData* filterData, const FilterInfo& filterInfo)
{
  FilterWidget* filterWidget = new FilterWidget(filterData, highlightingData, filterInfo, this);
  int index = traceWidgetTabWidget->addTab(filterWidget, filterData->getName());

  filterWidgetList.append(filterWidget);

  connect(filterWidget, &FilterWidget::autoscrollChanged, this, &TraceWindow::autoscrollChanged);
  connect(filterWidget, &FilterWidget::marksChanged, this, &TraceWindow::marksChanged);
  connect(filterWidget, &FilterWidget::nameChanged, this, &TraceWindow::onNameChanged);
  connect(filterWidget, &FilterWidget::traceWidgetChanged, this, &TraceWindow::traceWidgetChanged);
  connect(filterWidget, &FilterWidget::showInTraceRequested, this, &TraceWindow::showInTrace);
  connect(filterWidget, &FilterWidget::markInTraceRequested, this, &TraceWindow::markInTrace);
  connect(filterWidget, &FilterWidget::markAllInTraceRequested, this, &TraceWindow::markAllInTrace);
  connect(filterWidget, &FilterWidget::deleteTraceWidgetRequested, this, &TraceWindow::deleteFilter);
  connect(filterWidget, &FilterWidget::deleteTraceWindowRequested, this, &TraceWindow::deleteTraceWindowRequested);

  return filterWidget;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceWidget* TraceWindow::getCurrentTraceWidget()
{
  return qobject_cast<TraceWidget*>(traceWidgetTabWidget->getCurrentWidget());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QVector<FilterWidget*> TraceWindow::getFilterWidgetList() const
{
  return filterWidgetList;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int TraceWindow::getNewTraceNumber(const QString& name) const
{
  return traceWidgetTabWidget->getNewTabNumber(name);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int TraceWindow::getPosition(TraceWidget* traceWidget) const
{
  return traceWidgetTabWidget->getIndexOf(traceWidget);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SourceData* TraceWindow::getSourceData()
{
  return sourceWidget->getSourceData();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SourceWidget* TraceWindow::getSourceWidget()
{
  return sourceWidget;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceInfo TraceWindow::getTraceInfo() 
{
  TraceInfo traceInfo = sourceWidget->getTraceInfo();

  traceInfo.position = traceArea->getPosition(this);

  return traceInfo;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::setCurentIndex(int index)
{
  traceWidgetTabWidget->setCurrentIndex(index);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::setFont(const QFont& font)
{
  for (int i = 0; i < traceWidgetTabWidget->getCount(); ++i)
  {
    TraceWidget* traceWidget = qobject_cast<TraceWidget*>(traceWidgetTabWidget->getWidget(i));
    traceWidget->setFont(font);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::setHighlightingRules(const QVector<HighlightingRule*>& rules)
{
  for (int i = 0; i < traceWidgetTabWidget->getCount(); ++i)
  {
    TraceWidget* traceWidget = qobject_cast<TraceWidget*>(traceWidgetTabWidget->getWidget(i));
    traceWidget->setHighlightingRules(rules);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::showSource()
{
  traceWidgetTabWidget->setCurrentIndex(0);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::updateShortcuts()
{
  for (int i = 0; i < traceWidgetTabWidget->getCount(); ++i)
  {
    TraceWidget* traceWidget = qobject_cast<TraceWidget*>(traceWidgetTabWidget->getWidget(i));
    traceWidget->updateShortcuts();
  }
}


//===================================================================================================================================================================================================//
// PUBLIC SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::deleteFilter()
{
  TraceWidget* traceWidget = getCurrentTraceWidget();

  if (traceWidget->getType() == TraceWidget::Filter)
  {
    bool showDialog = Config().getBoolProperty(SETTINGS_DIALOG_DELETE_FILTER);
    bool ok = true;

    if (showDialog)
    {
      MessageDialog dialog(this, tr("Do you want to delete the filter <b><font color=#ff0000>") + traceWidget->getTraceData()->getName() + "</font></b>?", MessageDialog::Warning, true);
      ok = dialog.exec();

      Config().setProperty(SETTINGS_DIALOG_DELETE_FILTER, QString::number(!dialog.getDontAskAgain()));
    }

    if (ok)
    {
      FilterData* filterData = qobject_cast<FilterWidget*>(traceWidget)->getFilterData();
      QString filterId = filterData->getId();

      delete filterData;

      traceWidgetTabWidget->removeTab(traceWidgetTabWidget->getIndexOf(traceWidget));

      delete traceWidget;

      Config().deleteFilter(filterId);
    }
  }
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::cloneFilter()
{
  TraceWidget* traceWidget = getCurrentTraceWidget();

  if (traceWidget->getType() == TraceWidget::Filter)
  {
    FilterWidget* filterWidget = qobject_cast<FilterWidget*>(traceWidget);

    InputDialog dialog(this, tr("Clone Filter"), tr("Name"), filterWidget->getFilterInfo().name + tr("_clone"));
    bool ok = dialog.exec();

    if (ok)
    {
      FilterData* filterData = new FilterData(createId(), dialog.getTextValue(), sourceWidget->getSourceData());

      FilterWidget* newFilterWidget = addFilterWidget(filterData, filterWidget->getFilterInfo());

      Config().addFilter(newFilterWidget->getFilterInfo());
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::markAllInTrace(const QList<int>& lines)
{
  sourceWidget->markAllInTrace(lines);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::markInTrace(int line)
{
  sourceWidget->markInTrace(line);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::onNameChanged(TraceWidget* traceWidget, const QString& name)
{
  if (traceWidget->getType() == TraceWidget::Source)
  {
    emit nameChanged(this, name);
  }
  else
  {
    traceWidgetTabWidget->setTabText(traceWidgetTabWidget->getIndexOf(traceWidget), name);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::onTabMoved(int newIndex, int oldIndex)
{
  QVariantList ids;
  QVariantList positions;
  QVariantList traceIds;

  for (int i = 1; i < traceWidgetTabWidget->getCount(); ++i)
  {
    FilterWidget* filterWidget = qobject_cast<FilterWidget*>(traceWidgetTabWidget->getWidget(i));
    FilterInfo filterInfo = filterWidget->getFilterInfo();

    ids << filterInfo.id;
    positions << i;
    traceIds << sourceWidget->getSourceData()->getId();
  }

  Config().updateFilterPositions(ids, positions, traceIds);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::onTraceWidgetChanged(int index)
{
  Config().saveCurrentTraceWidgetIndex(getTraceInfo().id, traceWidgetTabWidget->getCurrentIndex());

  emit traceWidgetChanged(qobject_cast<TraceWidget*>(traceWidgetTabWidget->getCurrentWidget()));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::renameFilter()
{
  TraceWidget* traceWidget = getCurrentTraceWidget();

  if (traceWidget->getType() == TraceWidget::Filter)
  {
    FilterWidget* filterWidget = qobject_cast<FilterWidget*>(traceWidget);

    InputDialog dialog(this, tr("Rename Filter"), tr("Name"), filterWidget->getFilterInfo().name);
    bool ok = dialog.exec();

    if (ok)
    {
      FilterData* filterData = filterWidget->getFilterData();
      filterData->setName(dialog.getTextValue());

      Config().updateFilter(filterWidget->getFilterInfo());
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::showContextMenu(const QPoint& point)
{
  if (traceWidgetTabWidget->getTabAt(point) == 0)
  {
    return;
  }

  QAction* deleteFilterAction = new QAction(QIcon(":/Application/img/FilterDelete.png"), tr("Delete Filter"), this);
  connect(deleteFilterAction, &QAction::triggered, this, &TraceWindow::deleteFilter);

  QAction* renameFilterAction = new QAction(QIcon(":/Application/img/FilterRename.png"), tr("Rename Filter..."), this);
  connect(renameFilterAction, &QAction::triggered, this, &TraceWindow::renameFilter);

  QAction* cloneFilterAction = new QAction(QIcon(":/Application/img/FilterClone.png"), tr("Clone Filter..."), this);
  connect(cloneFilterAction, &QAction::triggered, this, &TraceWindow::cloneFilter);

  QMenu menu(this);
  menu.addAction(deleteFilterAction);
  menu.addAction(renameFilterAction);
  menu.addAction(cloneFilterAction);

  menu.exec(point);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWindow::showInTrace(int line)
{
  sourceWidget->goTo(line, 0);

  showSource();
}