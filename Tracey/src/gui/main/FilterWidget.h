#pragma once

#include <QFrame>

#include "src/gui/main/TraceWidget.h"

class FilterData;
class FilterPanel;

class QProgressBar;


class FilterWidget : public TraceWidget
{
  Q_OBJECT

public:
  FilterWidget(FilterData* filterData, HighlightingData* highlightingData, const FilterInfo& filterInfo, TraceWindow*  );

  void addOperator(const QChar& filterOperator);
  FilterData* getFilterData() const;
  FilterInfo getFilterInfo();
  FilterPanel* getFilterPanel();
  Type getType() const;
  void goTo(int line, int column) override;
  void toggleStartStop() override;

private:
  FilterData* filterData;
  FilterPanel* filterPanel;

private slots:
  void markAllInTrace();
  void markInTrace();
  void save() override;
  void showInTrace();
  
signals:
  void markAllInTraceRequested(const QList<int>&);
  void markInTraceRequested(int line);
  void showInTraceRequested(int line);

};