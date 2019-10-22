#include "src/gui/main/FilterWidget.h"

#include "src/logic/filter/FilterData.h"
#include "src/gui/core/Configuration.h"
#include "src/gui/main/FilterPanel.h"
#include "src/gui/widgets/traceviewer/TraceViewer.h"

#include <QAction>
#include <QDebug>
#include <QHBoxLayout>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterWidget::FilterWidget(FilterData* filterData, HighlightingData* highlightingData, const FilterInfo& filterInfo, TraceWindow* traceWindow) :
  TraceWidget(filterData, highlightingData, traceWindow),
  filterData(filterData)
{
  filterPanel = new FilterPanel(filterData, filterInfo, highlightingData, this);
  setTracePanel(filterPanel);

  QAction* showInTraceAction = new QAction(tr("Show In Trace"), this);
  connect(showInTraceAction, &QAction::triggered, this, &FilterWidget::showInTrace);

  QAction* markInTraceAction = new QAction(tr("Mark In Trace"), this);
  connect(markInTraceAction, &QAction::triggered, this, &FilterWidget::markInTrace);

  QAction* markAllInTraceAction = new QAction(tr("Mark All In Trace"), this);
  connect(markAllInTraceAction, &QAction::triggered, this, &FilterWidget::markAllInTrace);

  QAction* separatorAction = new QAction(this);
  separatorAction->setSeparator(true);

  QList<QAction*> actions;
  actions.append(showInTraceAction);
  actions.append(markInTraceAction);
  actions.append(markAllInTraceAction);
  actions.append(separatorAction);

  getTraceViewer()->setExtraContextMenuActions(actions);
  getTraceViewer()->setAutoscroll(filterInfo.autoscroll);
  getTraceViewer()->showSourceLines(filterInfo.sourceLines);

  connect(filterPanel, &FilterPanel::saveRequested, this, &FilterWidget::save);
  connect(filterPanel, &FilterPanel::sourceLinesToggled, getTraceViewer(), &TraceViewer::showSourceLines);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterWidget::addOperator(const QChar& filterOperator)
{
  filterPanel->addOperator(filterOperator);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterData* FilterWidget::getFilterData() const
{
  return filterData;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterInfo FilterWidget::getFilterInfo() 
{
  FilterInfo filterInfo;

  filterInfo.id = filterData->getId();
  filterInfo.traceId = filterData->getTraceId();
  filterInfo.name = filterData->getName();
  filterInfo.position = getPosition();
  filterInfo.searchType = filterPanel->getSearchType();
  filterInfo.matchCase = filterPanel->isMatchCase();
  filterInfo.wholeWord = filterPanel->isWholeWord();
  filterInfo.sourceLines = filterPanel->isSourceLines();
  filterInfo.extraLinesBefore = filterPanel->getExtraLinesBefore();
  filterInfo.extraLinesAfter = filterPanel->getExtraLinesAfter();
  filterInfo.expression = filterPanel->getFilterText();
  filterInfo.highlightingGroupId = filterPanel->getHighlightingGroupId();
  filterInfo.autoscroll = getTraceViewer()->isAutoscroll();
  filterInfo.operatorPositions = filterPanel->getOperatorPositions();

  return filterInfo;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceWidget::Type FilterWidget::getType() const
{
  return Filter;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterWidget::goTo(int line, int column)
{
  getTraceViewer()->goTo(line, column);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterWidget::toggleStartStop()
{
  filterPanel->toggleStartStop();
}

//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterWidget::markAllInTrace()
{
  emit markAllInTraceRequested(filterData->getLinesForMark(100));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterWidget::markInTrace()
{
  emit markInTraceRequested(filterData->getData(getTraceViewer()->getCursorLine()));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterWidget::save()
{
  Config().updateFilter(getFilterInfo());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterWidget::showInTrace()
{
  emit showInTraceRequested(filterData->getData(getTraceViewer()->getCursorLine()));
}