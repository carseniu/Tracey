#pragma once

#include <QFrame>

#include "src/gui/main/FilterWidget.h"

class Application;
class FilterData;
class HighlightingData; 
class HistoryModel;
class SourceData;
class TraceWidget;
class TraceWindow;
class TraceWindowTabWidget;

class QTabWidget;


class TraceArea : public QFrame
{
  Q_OBJECT

public:
  TraceArea(HighlightingData* highlightingData, Application* application);

  TraceWindow* addTraceWindow(SourceData* sourceData, const TraceInfo& traceInfo);
  TraceWidget* getCurrentTraceWidget();
  TraceWindow* getCurrentTraceWindow();
  int getNewTraceNumber(const QString& name) const;
  int getPosition(TraceWindow* traceWindow) const;
  void setCurentIndex(int index);
  void setFont(const QFont& font);
  void setHighlightingRules(const QVector<HighlightingRule*>& rules);
  void updateShortcuts();

private:
  HistoryModel* sourceHistoryModel;
  HighlightingData* highlightingData;
  TraceWindowTabWidget* traceWindowTabWidget;

public slots:
  void deleteTrace();

private slots:
  void cloneTrace();
  void deleteSourceHistory();
  void onTabMoved(int newIndex, int oldIndex);
  void onTraceWindowChanged(int index);
  void processNameChanged(TraceWindow* traceWindow, const QString& name);
  void renameTrace();
  void showContextMenu(const QPoint& point);

signals:
  void autoscrollChanged(bool autoscroll);
  void fileDropped(const QString& file);
  void traceWidgetChanged(TraceWidget* traceWidget);
  void traceWindowAdded(TraceWindow* traceWindow);

};
