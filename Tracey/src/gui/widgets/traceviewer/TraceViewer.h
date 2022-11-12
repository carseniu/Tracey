#pragma once

#include <QFrame>

#include "src/gui/core/HighlightingRule.h"

class HighlightingRule;
class StatusBarInfo;
class TraceData;
class Viewport;

class QScrollBar;
class QStatusBar;


class TraceViewer : public QFrame
{
  Q_OBJECT

public:
  TraceViewer(TraceData* traceData, QWidget* parent = nullptr);

  TraceData* getTraceData();

  int getCursorColumn() const;
  int getCursorLine() const;
  int getLineSize() const;
  void goTo(int line, int column = 0);
  bool isAutoscroll() const;
  void scroll(int lines);
  void setAutoscroll(bool autoscroll);
  void setExtraContextMenuActions(const QList<QAction*>& actions);
  void setFont(const QFont& font);
  void setHorizontalScrollBar(int max, int pageStep);
  void setHorizontalScrollBarValue(int value);
  void setVerticalScrollBarValue(int value);
  void showSourceLines(bool sourceLines);

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  TraceData* traceData;
  Viewport* viewport;
  QScrollBar* horizontalScrollBar;
  QScrollBar* verticalScrollBar;
  QStatusBar* statusBar;
  StatusBarInfo* statusBarInfo;

  bool autoscroll;
  int wheelScrollStep;

public slots:
  void setHighlightingRules(const QVector<HighlightingRule*>& rules);
  void showSearchResult(int line, int start, int end);

private slots:
  void disableAutoscroll();
  void onStarted();
  void onStopped();
  void processRowCountChanged(int rowCount);
  void processDeleted();
  void processUpdated();
  void scrollDown();
  void scrollUp();
  void update(int startLine);

signals:
  void autoscrollChanged(bool autoscroll);

};