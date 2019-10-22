#include "src/gui/core/TraceManager.h"

#include "src/Tracey.h"
#include "src/gui/core/Application.h"
#include "src/gui/core/Configuration.h"
#include "src/gui/dialogs/FontDialog.h"
#include "src/gui/dialogs/InputDialog.h"
#include "src/gui/dialogs/MessageDialog.h"
#include "src/gui/dialogs/SearchProgressDialog.h"
#include "src/gui/dialogs/SettingsDialog.h"
#include "src/gui/main/SourceWidget.h"
#include "src/gui/main/TraceArea.h"
#include "src/gui/main/TraceWidget.h"
#include "src/gui/main/TraceWindow.h"
#include "src/gui/widgets/traceviewer/TraceViewer.h"
#include "src/logic//filter/FilterData.h"
#include "src/logic/search/SearchWorkerCount.h"
#include "src/logic/search/SearchWorkerNext.h"
#include "src/logic/search/SearchWorkerPrevious.h"
#include "src/logic/source/SourceData.h"

#include <QDebug>
#include <QFileDialog>
#include <QThread>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceManager::TraceManager(TraceArea* traceArea, Application* application) :
  QObject(application),
  application(application),
  traceArea(traceArea),
  currentTraceWidget(nullptr),
  searchWorker(nullptr),
  searchProgressDialog(nullptr)
{
  connect(traceArea, &TraceArea::autoscrollChanged, this, &TraceManager::autoscrollChanged);
  connect(traceArea, &TraceArea::traceWindowAdded, this, &TraceManager::onTraceWindowAdded);
  connect(traceArea, &TraceArea::traceWidgetChanged, this, &TraceManager::traceWidgetChanged);
  connect(traceArea, &TraceArea::traceWidgetChanged, this, &TraceManager::processTraceWidgetChanged);
  connect(traceArea, &TraceArea::fileDropped, this, &TraceManager::onFileDropped);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::loadTraceConfig()
{
  int traceWindowIndex = Config().getCurrentTraceWindowIndex();

  QVector<TraceInfo> traces = Config().getTraces();

  for (TraceInfo traceInfo : traces)
  {
    int traceWidgetIndex = Config().getCurrentTraceWidgetIndex(traceInfo.id);

    SourceData* sourceData = new SourceData(traceInfo.id, traceInfo.name, this);
    TraceWindow* traceWindow = traceArea->addTraceWindow(sourceData, traceInfo);

    QVector<FilterInfo> filters = Config().getFilters(traceInfo.id);

    for (FilterInfo filterInfo : filters)
    {
      FilterData* filterData = new FilterData(filterInfo.id, filterInfo.name, sourceData);
      addFilterWidget(traceWindow, filterData, filterInfo);
    }

    traceWindow->setCurentIndex(traceWidgetIndex);
  }

  traceArea->updateShortcuts();
  traceArea->setCurentIndex(traceWindowIndex);
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterWidget* TraceManager::addFilterWidget(TraceWindow* parentTraceWindow, FilterData* filterData, const FilterInfo& filterInfo)
{
  FilterWidget* filterWidget = parentTraceWindow->addFilterWidget(filterData, filterInfo);

  return filterWidget;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::startSearch()
{
  if (searchWorker == nullptr)
  {
    return;
  }

  QThread* thread = new QThread;
  searchWorker->moveToThread(thread);

  connect(thread, &QThread::started, searchWorker, &SearchWorker::process);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);
  connect(searchWorker, &SearchWorker::finished, thread, &QThread::quit);
  connect(searchWorker, &SearchWorker::finished, searchWorker, &SearchWorker::deleteLater);
  connect(searchWorker, &SearchWorker::matchFound, currentTraceWidget->getTraceViewer(), &TraceViewer::showSearchResult);
  connect(searchWorker, &SearchWorker::matchFound, this, &TraceManager::processMatchFound);
  connect(searchWorker, &SearchWorker::matchNotFound, this, &TraceManager::processMatchNotFound);
  connect(searchWorker, &SearchWorker::updateReady, this, &TraceManager::processSearchUpdate);


  thread->start();
}

//===================================================================================================================================================================================================//
// PUBLIC SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::addFilter()
{
  TraceWindow* traceWindow = traceArea->getCurrentTraceWindow();
  int newFilterNumber = traceWindow->getNewTraceNumber(tr("Filter "));

  InputDialog dialog(application, tr("Add Filter"), tr("Name"), tr("Filter ").append(QString::number(newFilterNumber)));
  bool ok = dialog.exec();

  if (ok)
  {
    TraceWindow* traceWindow = traceArea->getCurrentTraceWindow();
    SourceData* sourceData = traceWindow->getSourceData();

    FilterData* filterData = new FilterData(createId(), dialog.getTextValue(), sourceData);

    FilterWidget* filterWidget = addFilterWidget(traceWindow, filterData);

    Config().addFilter(filterWidget->getFilterInfo());
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::addTrace()
{
  int newTraceNumber = traceArea->getNewTraceNumber(tr("Trace "));

  InputDialog dialog(application, tr("Add Trace"), tr("Name"), tr("Trace ").append(QString::number(newTraceNumber)));
  bool ok = dialog.exec();

  if (ok)
  {
    QString name = dialog.getTextValue();
    SourceData* sourceData = new SourceData(createId(), name, this);

    TraceInfo traceInfo;
    traceInfo.name = name;
    traceInfo.preloadContents = true;

    TraceWindow* traceWindow = traceArea->addTraceWindow(sourceData, traceInfo);

    Config().addTrace(traceWindow->getTraceInfo());
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::deleteAll()
{
  TraceWindow* traceWindow = traceArea->getCurrentTraceWindow();

  if (traceWindow != nullptr)
  {
    SourceData* sourceData = traceWindow->getSourceData();
    sourceData->reset();

    for (FilterWidget* filterWidget : traceWindow->getFilterWidgetList())
    {
      filterWidget->getFilterData()->reset();
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::deleteFilter()
{
  TraceWindow* traceWindow = traceArea->getCurrentTraceWindow();

  traceWindow->deleteFilter();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::deleteMarks()
{
  traceArea->getCurrentTraceWidget()->deleteMarks();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::deleteTrace()
{
  traceArea->deleteTrace();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::goToMark(int index)
{
  traceArea->getCurrentTraceWidget()->goTo(index, 0);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::onFileDropped(const QString& file)
{
  if (!file.isEmpty())
  {
    SourceData* sourceData = new SourceData(createId(), tr("Trace"), this);

    TraceInfo traceInfo;
    traceInfo.name = tr("Trace");
    traceInfo.preloadContents = true;

    TraceWindow* traceWindow = traceArea->addTraceWindow(sourceData, traceInfo);

    Config().addTrace(traceWindow->getTraceInfo());

    SourceWidget* sourceWidget = traceWindow->getSourceWidget();
    sourceWidget->addSource(file);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::processCount(const QRegularExpression& regexp)
{
  if (searchWorker != nullptr)
  {
    return;
  }

  searchWorker = new SearchWorkerCount(currentTraceWidget->getTraceData(), regexp);

  startSearch();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::processFindNext(const QRegularExpression& regexp)
{
  if (searchWorker != nullptr)
  {
    return;
  }

  searchWorker = new SearchWorkerNext(currentTraceWidget->getTraceData(), currentTraceWidget->getTraceViewer()->getCursorLine(), currentTraceWidget->getTraceViewer()->getCursorColumn(), regexp);

  startSearch();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::processFindPrevious(const QRegularExpression& regexp)
{
  if (searchWorker != nullptr)
  {
    return;
  }

  searchWorker = new SearchWorkerPrevious(currentTraceWidget->getTraceData(), currentTraceWidget->getTraceViewer()->getCursorLine(), currentTraceWidget->getTraceViewer()->getCursorColumn(), regexp);

  startSearch();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::processGoTo(int line, int column)
{
  if (currentTraceWidget)
  {
    currentTraceWidget->goTo(line - traceArea->getCurrentTraceWindow()->getSourceData()->getLineStart(), column);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::processTraceWidgetChanged(TraceWidget* traceWidget)
{
  if (currentTraceWidget != traceWidget)
  {
    currentTraceWidget = traceWidget;

    if (traceWidget != nullptr)
    {
      emit marksChanged(traceWidget->getMarks(), -1);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::save()
{
  TraceWidget* currentTraceWidget = traceArea->getCurrentTraceWidget();

  if (currentTraceWidget)
  {
    QString fileName = QFileDialog::getSaveFileName(application, tr("Save"), "D:/Work/Projects/_Test/Tracey/Save");

    if (!fileName.isEmpty())
    {
      traceArea->getCurrentTraceWidget()->saveAs(fileName);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::setAutoscroll(bool autoscroll)
{
  TraceWidget* currentTraceWidget = traceArea->getCurrentTraceWidget();

  if (currentTraceWidget)
  {
    traceArea->getCurrentTraceWidget()->setAutoscroll(autoscroll);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::showFontDialog()
{
  QFont font = FontDialog::getFont(QFont("Courier New", 9), true, false, application);

  traceArea->setFont(font);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::showSettingsDialog()
{
  if (settingsDialog == nullptr)
  {
    settingsDialog = new SettingsDialog(application);
  }

  settingsDialog->exec();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::toggleMark()
{
  traceArea->getCurrentTraceWidget()->toggleMark();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::toggleStartStop()
{
  if (currentTraceWidget == nullptr)
  {
    return;
  }

  currentTraceWidget->toggleStartStop();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::updateShortcuts()
{
  traceArea->updateShortcuts();
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::cancelSearch()
{
  if (searchWorker != nullptr)
  {
    searchWorker->setCanceled(true);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::onMarksChanged(int newIndex)
{
  emit marksChanged(currentTraceWidget->getMarks(), newIndex);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::onTraceWindowAdded(TraceWindow* traceWindow)
{
  connect(traceWindow, &TraceWindow::marksChanged, this, &TraceManager::onMarksChanged);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::processMatchFound()
{
  if (searchProgressDialog != nullptr)
  {
    searchProgressDialog->accept();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::processMatchNotFound()
{
  if (searchProgressDialog == nullptr)
  {
    searchProgressDialog = new SearchProgressDialog(application);
    searchProgressDialog->processMatchNotFound();
    searchProgressDialog->exec();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TraceManager::processSearchUpdate(int step, int count)
{
  if (searchProgressDialog == nullptr)
  {
    searchProgressDialog = new SearchProgressDialog(application);
    connect(searchProgressDialog, &SearchProgressDialog::rejected, this, &TraceManager::cancelSearch);
    connect(searchWorker, &SearchWorker::matchNotFound, searchProgressDialog, &SearchProgressDialog::processMatchNotFound);
    searchProgressDialog->setStep(step);
    searchProgressDialog->setCount(count);
    searchProgressDialog->exec();
  }
  else
  {
    searchProgressDialog->setStep(step);
    searchProgressDialog->setCount(count);
  }
}