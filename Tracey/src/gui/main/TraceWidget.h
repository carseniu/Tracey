#pragma once

#include <QFrame>

#include "src/Tracey.h"
#include "src/gui/core/HighlightingRule.h"
#include "src/logic/core/MarkData.h"

class HighlightingData;
class TraceData;
class TracePanel;
class TraceViewer;
class TraceWindow;

class QProgressBar;


class TraceWidget : public QFrame
{
  Q_OBJECT

public:
  enum Type {
    Source,
    Filter
  };

  TraceWidget(TraceData* traceData, HighlightingData* highlightingData, TraceWindow* traceWindow);

  void deleteMarks();
  const QVector<Mark>& getMarks() const;
  HighlightingData* getHighlightingData() const;
  int getPosition();
  TraceData* getTraceData();
  TraceViewer* getTraceViewer() const;
  virtual Type getType() const = 0;
  virtual void goTo(int line, int column) = 0;
  void saveAs(const QString& fileName);
  void setAutoscroll(bool enabled);
  void setFont(const QFont& font);
  void setHighlightingRules(const QVector<HighlightingRule*>& rules);
  void toggleMark();
  virtual void toggleStartStop() = 0;
  void updateShortcuts();

protected:
  bool eventFilter(QObject* object, QEvent* event) override;
  void setTracePanel(TracePanel* tracePanel);

private:
  void addChild(QObject* object);
  void removeChild(QObject* object);

  TraceWindow* traceWindow;

  TraceData* traceData;
  HighlightingData* highlightingData;

  TraceViewer* traceViewer;
  QProgressBar* progressBar;
  TracePanel* tracePanel;

private slots:
  void processHighlightingGroupChanged(const QString& groupId);
  void processNameChanged(const QString& name);
  void processContentProcessFinished();
  void processContentProcessStarted();
  void processStepReady(int step);
  virtual void save() = 0;

signals:
  void autoscrollChanged(bool autoscroll);
  void deleteTraceWidgetRequested();
  void deleteTraceWindowRequested();
  void marksChanged(int newIndex);
  void nameChanged(TraceWidget* traceWidget, const QString& name);
  void traceWidgetChanged(TraceWidget* traceWidget);

};