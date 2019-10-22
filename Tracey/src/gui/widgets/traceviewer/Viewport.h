#pragma once

#include <QPlainTextEdit>

#include <QMutex>

class HighlightingRule;
class Margin;
class TraceData;
class TraceViewer;
class TraceViewerHighlighter;

class QMenu;


class Viewport : public QPlainTextEdit
{
  Q_OBJECT

public:
  Viewport(TraceData* traceData, TraceViewer* parent);

  int getCursorColumn() const;
  int getCursorLine() const;
  int getLineSize() const;
  int getRowCount() const;
  int getStartLine() const;
  TraceData* getTraceData() const;
  void goTo(int line, int column = 0);
  void paintMargin(QPaintEvent* event);
  void reset();
  void setExtraContextMenuActions(const QList<QAction*>& actions);
  void setHighlightingRules(const QVector<HighlightingRule*>& rules);
  void setRowCount();
  void setViewportFont(const QFont& font);
  void showSearchResult(int line, int start, int end);
  void showSourceLines(bool sourceLines);
  void update();
  void update(int startLine);
  void updateHorizontalScrollBarMaximum();

protected:
  void contextMenuEvent(QContextMenuEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void mouseDoubleClickEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

private:
  void adjustHorizontalScrollBar();
  void clearSelection();
  int getEndIndexForLine(int line) const;
  QString getTextForCopy(const QString& line, int textSize = 0, bool* truncate = nullptr);
  void highlightCursorLine(QList<QTextEdit::ExtraSelection>& selections);
  void highlightDoubleClickSelection(QList<QTextEdit::ExtraSelection>& selections);
  void highlightSelection(QList<QTextEdit::ExtraSelection>& selections, const QStringList& lines);
  void processCopy();
  void processDown();
  void processEnd();
  void processHome();
  void processLeft();
  void processOutOfBoundsSelection();
  void processPageDown();
  void processPageUp();
  void processRight();
  void processSelectAll();
  void processUp();
  bool selectionExists() const;
  void setCursorColumn();
  void startSelectionTimer(int amouot);
  void stopSelectionTimer();
  void updateCursorPosition(const QStringList& lines);
  void updateMargin();

  static const QList<QChar> BORDER_CHARS;    // Characters to delimit a double click selection

  TraceViewer* traceViewer;
  TraceData* traceData;
  TraceViewerHighlighter* highlighter;
  Margin* margin;
  
  QMutex mutex;

  int cursorPosition;

  int rowCount;                         // The number of rows in the viewport, hight devided by line height, calculated in resizeEvent()
  int startLine;        
  int endLine;
  int lineCount;

  int cursorLine;                       // Cursor position in the real data
  int cursorColumn;

  int selectionStartLine;               // Selected text bounds
  int selectionStartColumn;
  int selectionEndLine;
  int selectionEndColumn;

  int anchorLine;                       // For mouse move selection
  int anchorColumn;

  bool shiftPressed;                    // For key selection
  bool controlPressed;

  bool updateInProgress;

  int maxLineSize;
  int maxLineIndex;
  int horizontalScrollBarValue;         // Preserve the scrollbar value when the content changes
  bool horizontalScrollBarAdjustment;

  int marginOffset;
  int horizontalScrollBarOffset;

  QString doubleClickSelection;

  // Colors
  QString textColor;
  QString backgroundColor;
  QString currentLineColor;
  QString selectionColor;
  QString doubleClickSelectionColor;

  QMenu* contextMenu;

  QTimer* selectionTimer;
  int outOfBoundsScrollAmount;
  int outOfBoundsScrollRate;

  int maxCharacterCountForCopy;

  int searchResultLength;

  bool sourceLines;

public slots:
  void onMarksChanged();
  void setHorizontalScrollBarValue(int value);

private slots:
  void copyMarkedLines();
  void onMarkToggled(int index);
  void setOutOfBoundsSelection();

signals:
  void cursorChanged(int line, int column);
  void rowCountChanged(int rowCount);
  void scrollDownRequested();
  void scrollUpRequested();
  void selectionChanged(int size, int columns);

};