#include "src/gui/main/TraceWidget.h"

#include "src/gui/core/HighlightingData.h"
#include "src/gui/main/TracePanel.h"
#include "src/gui/main/TraceWindow.h"
#include "src/gui/widgets/traceviewer/TraceViewer.h"
#include "src/logic/core/TraceData.h"

#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QSplitter>
#include <QVBoxLayout>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceWidget::TraceWidget(TraceData* traceData, HighlightingData* highlightingData, TraceWindow* traceWindow) :
  QFrame(traceWindow),
  traceWindow(traceWindow),
  traceData(traceData),
  highlightingData(highlightingData)
{
  traceViewer = new TraceViewer(traceData, this);
  traceViewer->setAutoscroll(true);

  progressBar = new QProgressBar(this);
  progressBar->setObjectName("TraceWidgetProgressBar");
  progressBar->setTextVisible(false);
  progressBar->setFixedHeight(10);
  progressBar->setMaximum(100);
  progressBar->setMinimum(0);
  progressBar->setValue(0);
  progressBar->setVisible(false);

  installEventFilter(this);

  connect(traceData, &TraceData::nameChanged, this, &TraceWidget::processNameChanged);
  connect(traceData, &TraceData::contentProcessStarted, this, &TraceWidget::processContentProcessStarted);
  connect(traceData, &TraceData::contentProcessFinished, this, &TraceWidget::processContentProcessFinished);
  connect(traceData, &TraceData::stepReady, this, &TraceWidget::processStepReady);
  connect(traceData, &TraceData::marksChanged, this, &TraceWidget::marksChanged);
  connect(traceViewer, &TraceViewer::autoscrollChanged, this, &TraceWidget::autoscrollChanged);
  connect(traceViewer, &TraceViewer::autoscrollChanged, this, &TraceWidget::save);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::deleteMarks()
{
  traceData->deleteMarks();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
const QVector<Mark>& TraceWidget::getMarks() const
{
  return traceData->getMarks();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
HighlightingData* TraceWidget::getHighlightingData() const
{
  return highlightingData;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int TraceWidget::getPosition()
{
  return traceWindow->getPosition(this);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceData* TraceWidget::getTraceData()
{
  return traceData;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceViewer* TraceWidget::getTraceViewer() const
{
  return traceViewer;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::saveAs(const QString& fileName)
{
  traceData->saveAs(fileName);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::setAutoscroll(bool enabled)
{
  traceViewer->setAutoscroll(enabled);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::setFont(const QFont& font)
{
  traceViewer->setFont(font);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::setHighlightingRules(const QVector<HighlightingRule*>& rules)
{
  traceViewer->setHighlightingRules(rules);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::toggleMark()
{
  traceData->toggleMark(traceViewer->getCursorLine());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::updateShortcuts()
{
  tracePanel->updateShortcuts();
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool TraceWidget::eventFilter(QObject* object, QEvent* event)
{
  switch (event->type())
  {
    case QEvent::ChildAdded:
    {
      QChildEvent* childEvent = dynamic_cast<QChildEvent*>(event);
      addChild(childEvent->child());
    }
    break;

    case QEvent::ChildRemoved:
    {
      QChildEvent* childEvent = dynamic_cast<QChildEvent*>(event);
      removeChild(childEvent->child());
    }
    break;

    case QEvent::MouseButtonPress:
    {
      emit traceWidgetChanged(this);
    }
    break;
  }

  return QWidget::eventFilter(object, event);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::setTracePanel(TracePanel* tracePanel)
{
  this->tracePanel = tracePanel;

  QWidget* bottomPanel = new QWidget(this);
  QVBoxLayout* bottomPanelLayout = new QVBoxLayout(bottomPanel);
  bottomPanelLayout->setMargin(0);
  bottomPanelLayout->setSpacing(0);
  bottomPanelLayout->addWidget(progressBar);
  bottomPanelLayout->addWidget(traceViewer);


  QSplitter* splitter = new QSplitter(Qt::Vertical, this);
  splitter->addWidget(tracePanel);
  splitter->addWidget(bottomPanel);
  splitter->setSizes(QList<int>() << 200 << 10000);

  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(splitter);

  connect(tracePanel, &TracePanel::highlightingGroupChanged, this, &TraceWidget::processHighlightingGroupChanged, Qt::QueuedConnection);
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::addChild(QObject* object)
{
  if (object && object->isWidgetType())
  {
    object->installEventFilter(this);

    const QObjectList& childList = object->children();
    for (QObjectList::const_iterator it = childList.begin(); it != childList.end(); ++it)
    {
      addChild(*it);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::removeChild(QObject* object)
{
  if (object && object->isWidgetType())
  {
    object->removeEventFilter(this);

    const QObjectList& childList = object->children();
    for (QObjectList::const_iterator it = childList.begin(); it != childList.end(); ++it)
    {
      removeChild(*it);
    }
  }
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::processHighlightingGroupChanged(const QString& groupId)
{
  if (groupId.isEmpty()) {
    traceViewer->setHighlightingRules(QVector<HighlightingRule*>());
  }
  else {
    traceViewer->setHighlightingRules(highlightingData->getRules(groupId));
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::processNameChanged(const QString& name)
{
  emit nameChanged(this, name);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::processContentProcessFinished()
{
  progressBar->setVisible(false);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::processContentProcessStarted()
{
  progressBar->setVisible(true);
  progressBar->setValue(0);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceWidget::processStepReady(int step)
{
  progressBar->setValue(step);
}