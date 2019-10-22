#pragma once

#include <QFrame>

#include "TraceWidget.h"

class HistoryModel;
class SourceData;
class SourcePanel;

class QProgressBar;


class SourceWidget : public TraceWidget
{
  Q_OBJECT

public:
  SourceWidget(SourceData* sourceData, HighlightingData* highlightingData, HistoryModel* historyModel, const TraceInfo& traceInfo, TraceWindow* traceWindow);

  void markAllInTrace(const QList<int>& lines);
  void markInTrace(int line);
  void addSource(const QString& file);
  SourceData* getSourceData() const;
  TraceInfo getTraceInfo();
  int getTraceViewerLineSize() const;
  Type getType() const;
  void goTo(int line, int column) override;
  void toggleStartStop() override;

private:
  SourceData* sourceData;
  SourcePanel* sourcePanel;

private slots:
  void save() override;

signals:
  void deleteSourceHistoryRequested();

};