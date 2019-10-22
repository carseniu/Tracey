#pragma once

#include <QObject>

#include "src/gui/core/HighlightingRule.h"
#include "src/gui/main/FilterWidget.h"
#include "src/logic/core/MarkData.h"

#include <QHash>
#include <QPointer>
#include <QVector>

class Application;
class FilterData;
class SearchProgressDialog;
class SearchWorker;
class SettingsDialog;
class SourceData;
class SourceReader;
class TraceArea;
class TraceWidget;
class TraceWindow;


class TraceManager : public QObject
{
  Q_OBJECT

public:
  TraceManager(TraceArea* traceArea, Application* application);
  void loadTraceConfig();

private:
  FilterWidget* addFilterWidget(TraceWindow* parentTraceWindow, FilterData* filterData, const FilterInfo& filterInfo = FilterInfo());
  void startSearch();

  Application* application;
  TraceArea* traceArea;
  SettingsDialog* settingsDialog = nullptr;

  TraceWidget* currentTraceWidget;

  QPointer<SearchWorker> searchWorker;  
  QPointer<SearchProgressDialog> searchProgressDialog;

public slots:
  void addFilter();
  void addTrace();
  void deleteAll();
  void deleteFilter();
  void deleteMarks();
  void deleteTrace();
  void goToMark(int index);
  void onFileDropped(const QString& file);
  void processCount(const QRegularExpression& regexp);
  void processFindNext(const QRegularExpression& regexp);
  void processFindPrevious(const QRegularExpression& regexp);
  void processGoTo(int line, int column);
  void processTraceWidgetChanged(TraceWidget* traceWidget);
  void save();
  void setAutoscroll(bool autoscroll);
  void showFontDialog();
  void showSettingsDialog();
  void toggleMark();
  void toggleStartStop();
  void updateShortcuts();

private slots:
  void cancelSearch();
  void onMarksChanged(int newIndex);
  void onTraceWindowAdded(TraceWindow* traceWindow);
  void processMatchFound();
  void processMatchNotFound();
  void processSearchUpdate(int step, int count);

signals:
  void autoscrollChanged(bool autoscroll);
  void marksChanged(const QVector<Mark>& marks, int newIndex);
  void traceWidgetChanged(TraceWidget* traceWidget);

};

