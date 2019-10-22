#pragma once

#include <QFrame>

#include "src/gui/core/HighlightingRule.h"
#include "src/gui/main/TraceWidget.h"

class FilterData;
class FilterWidget;
class HighlightingData;
class HistoryModel;
class SourceData;
class SourceWidget;
class TraceArea;
class TraceTabWidget;
class TraceWidget;


class TraceWindow : public QFrame
{
  Q_OBJECT

public:
  TraceWindow(SourceData* sourceData, HighlightingData* highlightingData, HistoryModel* historyModel, const TraceInfo& traceInfo, TraceArea* traceArea);
  
  FilterWidget* addFilterWidget(FilterData* filterData, const FilterInfo& fitlerInfo);
  TraceWidget* getCurrentTraceWidget();
  QVector<FilterWidget*> getFilterWidgetList() const;
  int getNewTraceNumber(const QString& name) const;
  int getPosition(TraceWidget* traceWidget) const;
  SourceData* getSourceData();
  SourceWidget* getSourceWidget();
  TraceInfo getTraceInfo();
  void setCurentIndex(int index);
  void setFont(const QFont& font);
  void setHighlightingRules(const QVector<HighlightingRule*>& rules);
  void showSource();
  void updateShortcuts();

private:
  TraceArea* traceArea;

  HighlightingData* highlightingData;
  TraceTabWidget* traceWidgetTabWidget;

  SourceWidget* sourceWidget;
  QVector<FilterWidget*> filterWidgetList;

public slots:
  void deleteFilter();

private slots:
  void cloneFilter();
  void markAllInTrace(const QList<int>& lines);
  void markInTrace(int line);
  void onNameChanged(TraceWidget* traceWidget, const QString& name);
  void onTabMoved(int newIndex, int oldIndex);
  void onTraceWidgetChanged(int index);
  void renameFilter();
  void showContextMenu(const QPoint& point);
  void showInTrace(int line);

signals:
  void autoscrollChanged(bool autoscroll);
  void deleteSourceHistoryRequested();
  void deleteTraceWindowRequested();
  void marksChanged(int newIndex);
  void nameChanged(TraceWindow* traceWidget, const QString& name);
  void traceWidgetChanged(TraceWidget* traceWidget);

};
