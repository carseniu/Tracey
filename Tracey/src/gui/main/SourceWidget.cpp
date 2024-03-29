#include "src/gui/main/SourceWidget.h"

#include "src/gui/core/Configuration.h"
#include "src/gui/main/SourcePanel.h"
#include "src/gui/main/TraceWindow.h"
#include "src/gui/widgets/traceviewer/TraceViewer.h"
#include "src/logic/source/SourceData.h"

#include <QDebug>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SourceWidget::SourceWidget(SourceData* sourceData, HighlightingData* highlightingData, HistoryModel* historyModel, const TraceInfo& traceInfo, TraceWindow* traceWindow) :
  TraceWidget(sourceData, highlightingData, traceWindow),
  sourceData(sourceData)
{
  sourcePanel = new SourcePanel(sourceData, traceInfo, highlightingData, historyModel, this);
  setTracePanel(sourcePanel);

  getTraceViewer()->setAutoscroll(traceInfo.autoscroll);

  connect(sourcePanel, &SourcePanel::saveRequested, this, &SourceWidget::save);
  connect(sourcePanel, &SourcePanel::deleteSourceHistoryRequested, this, &SourceWidget::deleteSourceHistoryRequested);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourceWidget::markAllInTrace(const QList<int>& lines)
{
  sourceData->addMarks(lines);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourceWidget::markInTrace(int line)
{
  sourceData->addMark(line);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourceWidget::addSource(const QString& file)
{
  sourcePanel->addSource(file);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SourceData* SourceWidget::getSourceData() const
{
  return sourceData;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceInfo SourceWidget::getTraceInfo()
{
  TraceInfo traceInfo;

  traceInfo.id = sourceData->getId();
  traceInfo.name = sourceData->getName();
  traceInfo.position = getPosition();
  traceInfo.source = sourcePanel->getSource();
  traceInfo.rangeType = sourcePanel->getRangeType();
  traceInfo.openStart = sourcePanel->getOpenStart();
  traceInfo.openEnd = sourcePanel->getOpenEnd();
  traceInfo.lineHandling = sourcePanel->getLineHandling();
  traceInfo.lineSize = sourcePanel->getLineSize();
  traceInfo.preloadContents = sourcePanel->isPreloadFileContents();

  traceInfo.highlightingGroupId = sourcePanel->getHighlightingGroupId();
  traceInfo.autoscroll = getTraceViewer()->isAutoscroll();

  return traceInfo;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int SourceWidget::getTraceViewerLineSize() const
{
  return getTraceViewer()->getLineSize();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceWidget::Type SourceWidget::getType() const
{
  return Source;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourceWidget::goTo(int line, int column)
{
  getTraceViewer()->goTo(line, column);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourceWidget::toggleStartStop()
{
  sourcePanel->toggleStartStop();
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourceWidget::save()
{
  Config().updateTrace(getTraceInfo());
}
