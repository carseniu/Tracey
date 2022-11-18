#include "src/gui/widgets/traceviewer/Viewport.h"

#include "src/gui/widgets/traceviewer/Margin.h"
#include "src/gui/widgets/traceviewer/TraceViewer.h"
#include "src/gui/widgets/traceviewer/TraceViewerHighlighter.h"
#include "src/logic/core/TraceData.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QMenu>
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>
#include <QThreadPool>
#include <QTimer>

#include <utility>

const QList<QChar> Viewport::BORDER_CHARS = QList<QChar>()
<< ' ' << '`' << '~' << '!' << '@' << '#' << '$' << '%' << '^' << '&' << '*' << '(' << ')'
<< '|' << '\\' << '|' << '/' << '[' << ']' << '{' << '}' << ':' << ';' << '"' << '\'' << '?'
<< ',' << '.' << '<' << '>' << '-' << '+' << '=';


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
Viewport::Viewport(TraceData* traceData, TraceViewer* traceViewer) :
  QPlainTextEdit(traceViewer),
  traceViewer(traceViewer),
  traceData(traceData),
  cursorPosition(-1),
  rowCount(0),
  startLine(0),
  endLine(0),
  lineCount(0),
  cursorLine(-1),
  cursorColumn(-1),
  selectionStartLine(-1),
  selectionStartColumn(-1),
  selectionEndLine(-1),
  selectionEndColumn(-1),
  anchorLine(-1),
  anchorColumn(-1),
  shiftPressed(false),
  controlPressed(false),
  maxLineSize(0),
  maxLineIndex(-1),
  horizontalScrollBarValue(0),
  horizontalScrollBarAdjustment(false),
  marginOffset(25),
  horizontalScrollBarOffset(20),
  textColor("#000000"),
  currentLineColor("#ffdb92"),
  selectionColor("#b7dbff"),
  doubleClickSelectionColor("#e3cee6"),
  outOfBoundsScrollAmount(0),
  outOfBoundsScrollRate(50),
  maxCharacterCountForCopy(10000000),
  doubleClickSelection(""),
  searchResultLength(0),
  sourceLines(false)
{
  setFont(QFont("Courier New", 9));
  setLineWrapMode(QPlainTextEdit::NoWrap);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setReadOnly(true);
  setTextInteractionFlags(textInteractionFlags() | Qt::TextSelectableByKeyboard);
  setStyleSheet("border:1px solid transparent; selection-background-color:transparent");
  setCursor(Qt::IBeamCursor);

  QPalette palette = this->palette();
  palette.setColor(QPalette::Base, Qt::white);
  palette.setColor(QPalette::Text, Qt::black);
  this->setPalette(palette);

  highlighter = new TraceViewerHighlighter(document());

  margin = new Margin(this);
  margin->setFixedSize(QSize(0, 0));
  connect(margin, &Margin::markToggled, this, &Viewport::onMarkToggled);

  QAction* copyMarkedLinesAction = new QAction(tr("Copy Marked Lines"), this);
  connect(copyMarkedLinesAction, &QAction::triggered, this, &Viewport::copyMarkedLines);

  contextMenu = new QMenu(this);
  contextMenu->addAction(copyMarkedLinesAction);

  selectionTimer = new QTimer(this);
  connect(selectionTimer, &QTimer::timeout, this, &Viewport::setOutOfBoundsSelection);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int Viewport::getCursorColumn() const
{
  return cursorColumn;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int Viewport::getCursorLine() const
{
  return cursorLine;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int Viewport::getLineSize() const
{
  return width() / fontMetrics().horizontalAdvance(" ") - 15;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int Viewport::getRowCount() const
{
  return rowCount;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int Viewport::getStartLine() const
{
  return startLine;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TraceData* Viewport::getTraceData() const
{
  return traceData;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::goTo(int line, int column)
{
  int dataSize = traceData->getSize();

  if (dataSize == 0)
  {
    return;
  }

  if (line < 0)
  {
    line = 0;
  }

  if (line > dataSize - 1)
  {
    line = dataSize - 1;
  }

  int newStartLine;

  // If the line is not visible, move the viewport, so that the line is centered, if possible
  if (line < startLine || line > endLine)
  {
    int screenMiddle = rowCount / 2;

    if (line - screenMiddle < 0)
    {
      newStartLine = 0;
    }
    else if (line + screenMiddle > dataSize - 1)
    {
      newStartLine = dataSize - rowCount;
    }
    else
    {
      newStartLine = line - screenMiddle;
    }
  }
  else
  {
    newStartLine = startLine;
  }

  cursorLine = line;
  cursorColumn = column - 1;

  if (newStartLine != startLine)
  {
    startLine = newStartLine;
    traceViewer->setVerticalScrollBarValue(startLine);
  }

  horizontalScrollBarAdjustment = true;
  update(startLine);

  setFocus();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::paintMargin(QPaintEvent* event)
{
  QPainter painter(margin);
  painter.setFont(font());
  painter.setPen(Qt::black);

  int rightMargin = 20;
  int blockCount = 0;

  margin->clearIndexMap();

  for (int i = startLine; i < startLine + lineCount; ++i)
  {
    QTextBlock block = document()->findBlockByNumber(blockCount++);

    if (block.isValid() && block.isVisible())
    {
      QRectF blockRect = blockBoundingGeometry(block).translated(contentOffset());

      painter.drawText(0, blockRect.top() + 1, margin->width() - rightMargin, fontMetrics().height(), Qt::AlignRight, traceData->getLineNumber(i, sourceLines));

      QRect rect(0, blockRect.top() + 1, margin->width() - 1, fontMetrics().height());
      margin->addIndex(i, rect);

      if (traceData->markExists(i))
      {
        QPixmap pixmap(":/Application/img/Mark.png");

        painter.drawPixmap(margin->width() - rightMargin + 5, blockRect.top() + 1 + (fontMetrics().height() - pixmap.height()) / 2, pixmap);
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::reset()
{
  setEnabled(false);
  clear();

  maxLineSize = 0;
  maxLineIndex = -1;
  horizontalScrollBarValue = 0;

  cursorPosition = -1;
  cursorLine = -1;
  cursorColumn = -1;
  selectionStartLine = -1;
  selectionStartColumn = -1;
  selectionEndLine = -1;
  selectionEndColumn = -1;
  anchorLine = -1;
  anchorColumn = -1;
  doubleClickSelection = "";

  setViewportMargins(0, 0, 0, 0);
  margin->setFixedSize(QSize(0, this->height()));
  margin->update();

  update();

  emit cursorChanged(cursorLine, cursorColumn);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::setExtraContextMenuActions(const QList<QAction*>& actions)
{
  contextMenu->insertActions(contextMenu->actions().first(), actions);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::setHighlightingRules(const QVector<HighlightingRule*>& rules)
{
  highlighter->setRules(rules);

  update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::setRowCount()
{
  rowCount = (viewport()->height() - 10) / fontMetrics().height();

  if (rowCount < 0)
  {
    rowCount = 0;
  }

  emit rowCountChanged(rowCount);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::setViewportFont(const QFont& font)
{
  setFont(font);

  setRowCount();

  updateHorizontalScrollBarMaximum();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::showSearchResult(int line, int start, int end)
{
  selectionStartLine = line;
  selectionEndLine = line;
  selectionStartColumn = start;
  selectionEndColumn = end;
  searchResultLength = fontMetrics().averageCharWidth() * (end - start);

  goTo(line, end + 1);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::showSourceLines(bool sourceLines)
{
  this->sourceLines = sourceLines;

  update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::update()
{
  update(startLine);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::update(int startLine)
{
  QStringList lines = traceData->getLines(startLine, rowCount);
  lineCount = lines.size();

  clear();

  this->startLine = startLine;
  this->endLine = startLine + lines.size() - 1;

  QString page = "";

  for (int i = 0; i < lines.size(); ++i)
  {
    page.append(lines.at(i));

    if (i < lines.size() - 1)
    {
      page.append('\n');
    }

    int lineSize = fontMetrics().horizontalAdvance(lines.at(i));
    if (lineSize > maxLineSize)
    {
      maxLineSize = lineSize;
      maxLineIndex = startLine + i;
    }
  }

  appendPlainText(page);

  if (lines.size() > 0 && cursorLine == -1) {
    setEnabled(true);
    cursorLine = 0;
    cursorColumn = 0;
  }

  processOutOfBoundsSelection();

  // First move the cursor to highlight the cursor line
  updateCursorPosition(lines);

  QList<QTextEdit::ExtraSelection> selections;
  highlightCursorLine(selections);
  highlightDoubleClickSelection(selections);
  highlightSelection(selections, lines);
  setExtraSelections(selections);

  // Update the cursor position after the whole highlighting
  updateCursorPosition(lines);

  updateHorizontalScrollBarMaximum();
  adjustHorizontalScrollBar();

  updateMargin();

  if (!page.isEmpty())
  {
    emit cursorChanged(traceData->getLineNumber(cursorLine), cursorColumn + 1);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::updateHorizontalScrollBarMaximum()
{
  if (maxLineIndex != -1)
  {
    int lineSize = fontMetrics().horizontalAdvance(traceData->getLine(maxLineIndex));

    int horizontalScrollBarMaximum = lineSize - width() + margin->width() + horizontalScrollBarOffset;
    if (horizontalScrollBarMaximum < 0)
    {
      if (lineSize > width() - margin->width() - horizontalScrollBarOffset)
      {
        horizontalScrollBarMaximum = width() - margin->width() - horizontalScrollBarOffset - lineSize;
        viewport()->setFixedWidth(lineSize + horizontalScrollBarOffset);
      }
      else
      {
        horizontalScrollBarMaximum = 0;
        viewport()->setFixedWidth(width());
      }
    }
    else
    {
      viewport()->setFixedWidth(lineSize + horizontalScrollBarOffset);
    }

    int pageStep = width() - margin->width();

    horizontalScrollBar()->setMaximum(horizontalScrollBarMaximum);
    horizontalScrollBar()->setPageStep(pageStep);
    traceViewer->setHorizontalScrollBar(horizontalScrollBarMaximum, pageStep);
    horizontalScrollBar()->setValue(horizontalScrollBarValue);
  }
  else
  {
    horizontalScrollBar()->setMaximum(0);
    horizontalScrollBar()->setPageStep(0);
    traceViewer->setHorizontalScrollBar(0, 0);
  }
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::contextMenuEvent(QContextMenuEvent* event)
{
  if (traceData->getSize() == 0 || contextMenu->actions().isEmpty())
  {
    return;
  }

  contextMenu->exec(event->globalPos());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::keyPressEvent(QKeyEvent* event)
{
  // Shift
  if ((event->key() == Qt::Key_Shift))
  {
    shiftPressed = true;

    if (!selectionExists())
    {
      selectionStartLine = cursorLine;
      selectionStartColumn = cursorColumn;
    }
  }
  // Control
  else if ((event->key() == Qt::Key_Control))
  {
    controlPressed = true;
  }

  // Up
  if ((event->key() == Qt::Key_Up))
  {
    processUp();
  }
  // Down
  else if ((event->key() == Qt::Key_Down))
  {
    processDown();
  }
  // Left
  else if ((event->key() == Qt::Key_Left))
  {
    processLeft();
  }
  // Right
  else if ((event->key() == Qt::Key_Right))
  {
    processRight();
  }
  // Page Down
  else if ((event->key() == Qt::Key_PageDown))
  {
    processPageDown();
  }
  // Page Up
  else if ((event->key() == Qt::Key_PageUp))
  {
    processPageUp();
  }
  // Home
  else if (event->key() == Qt::Key_Home)
  {
    processHome();
  }
  // End
  else if (event->key() == Qt::Key_End)
  {
    processEnd();
  }
  // Select All
  else if (event->key() == Qt::Key_A && (event->modifiers() & Qt::ControlModifier))
  {
    processSelectAll();
  }
  // Copy
  else if (((event->key() == Qt::Key_C) && (event->modifiers() & Qt::ControlModifier)) || ((event->key() == Qt::Key_Insert) && (event->modifiers() & Qt::ControlModifier)))
  {
    processCopy();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::keyReleaseEvent(QKeyEvent* event)
{
  // Shift
  if ((event->key() == Qt::Key_Shift))
  {
    shiftPressed = false;
  }
  // Control
  else if ((event->key() == Qt::Key_Control))
  {
    controlPressed = false;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::mouseDoubleClickEvent(QMouseEvent* event)
{
  QTextCursor tc = cursorForPosition(event->pos());
  tc.select(QTextCursor::WordUnderCursor);

  if (!tc.selectedText().isEmpty())
  {
    doubleClickSelection = tc.selectedText();

    selectionStartLine = cursorLine;
    selectionStartColumn = tc.selectionStart() - tc.block().position();
    selectionEndLine = cursorLine;
    selectionEndColumn = tc.selectionEnd() - tc.block().position();
  }
  else
  {
    doubleClickSelection = "";

    selectionStartLine = -1;
    selectionStartColumn = -1;
    selectionEndLine = -1;
    selectionEndColumn = -1;
  }

  update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::mouseMoveEvent(QMouseEvent* event)
{
  if (traceData->getSize() == 0)
  {
    return;
  }

  if (!selectionTimer->isActive())
  {
    QPlainTextEdit::mousePressEvent(event);
    QTextCursor tc = textCursor();

    if (tc.position() != cursorPosition)
    {
      cursorLine = startLine + tc.blockNumber();
      if (cursorLine >= traceData->getSize() - 1)
      {
        cursorLine = traceData->getSize() - 1;
      }

      cursorColumn = tc.columnNumber();

      if (cursorLine < anchorLine || (cursorLine == anchorLine && tc.columnNumber() < anchorColumn))
      {
        selectionStartLine = cursorLine;
        selectionStartColumn = tc.columnNumber();
        selectionEndLine = anchorLine;
        selectionEndColumn = anchorColumn;
      }
      else
      {
        selectionStartLine = anchorLine;
        selectionStartColumn = anchorColumn;
        selectionEndLine = cursorLine;
        selectionEndColumn = tc.columnNumber();
      }

      cursorPosition = tc.position();

      update();
    }

    if (event->y() > height() && endLine < traceData->getSize() - 1)
    {
      startSelectionTimer(1);
    }
    else if (event->y() < 0 && startLine > 0)
    {
      startSelectionTimer(-1);
    }
  }
  else if (event->y() > 0 && event->y() < height()) {
    stopSelectionTimer();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::mousePressEvent(QMouseEvent* event)
{
  if (traceData->getSize() == 0)
  {
    return;
  }

  traceViewer->setAutoscroll(false);

  QTextCursor tc;

  if (event->button() == Qt::LeftButton)
  {
    clearSelection();

    QPlainTextEdit::mousePressEvent(event);

    tc = textCursor();
  }
  else if (event->button() == Qt::RightButton)
  {
    tc = cursorForPosition(event->pos());
  }

  cursorPosition = tc.position();

  cursorLine = startLine + tc.blockNumber();
  if (cursorLine >= traceData->getSize() - 1)
  {
    cursorLine = traceData->getSize() - 1;
  }

  cursorColumn = tc.columnNumber();

  anchorLine = cursorLine;
  anchorColumn = tc.columnNumber();

  doubleClickSelection = "";

  update(startLine);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::mouseReleaseEvent(QMouseEvent* event)
{
  if (traceData->getSize() == 0)
  {
    return;
  }

  if (selectionTimer->isActive())
  {
    stopSelectionTimer();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::resizeEvent(QResizeEvent* event)
{
  if (event->size().height() != event->oldSize().height())
  {
    setRowCount();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::wheelEvent(QWheelEvent* event)
{
  traceViewer->setAutoscroll(false);

  // Change font size
  if (event->modifiers() & Qt::ControlModifier)
  {
    QFontDatabase fdb;
    QFont newFont(font());
    QList<int> fontSizes = fdb.pointSizes(newFont.family(), newFont.styleName());

    int startIndex = fontSizes.indexOf(newFont.pointSize());

    if (event->angleDelta().y() > 0)
    {
      if (startIndex < fontSizes.size() - 1)
      {
        newFont.setPointSize(fontSizes.at(++startIndex));
        setViewportFont(newFont);
      }
    }
    else
    {
      if (startIndex > 0)
      {
        newFont.setPointSize(fontSizes.at(--startIndex));
        setViewportFont(newFont);
      }
    }
  }
  // Scroll
  else
  {
    if (event->angleDelta().y() > 0)
    {
      emit scrollUpRequested();
    }
    else
    {
      emit scrollDownRequested();
    }
  }
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::adjustHorizontalScrollBar()
{
  if (horizontalScrollBarAdjustment)
  {
    int cursorRight = cursorRect().right() + horizontalScrollBarValue;
    int availableWidth = width() - margin->width();

    if (cursorRight > availableWidth + horizontalScrollBarValue)
    {
      traceViewer->setHorizontalScrollBarValue(cursorRight - availableWidth + horizontalScrollBarOffset);
    }
    else if (cursorRight <= horizontalScrollBarValue)
    {
      int value = cursorRight - horizontalScrollBarOffset - searchResultLength;
      if (value > horizontalScrollBarValue + availableWidth - horizontalScrollBarOffset)
      {
        value = cursorRight - availableWidth + horizontalScrollBarOffset;
      }

      traceViewer->setHorizontalScrollBarValue(value);
    }

    horizontalScrollBarAdjustment = false;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::clearSelection()
{
  if (selectionExists())
  {
    selectionStartLine = -1;
    selectionStartColumn = -1;
    selectionEndLine = -1;
    selectionEndColumn = -1;
    searchResultLength = 0;

    update();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int Viewport::getEndIndexForLine(int line) const
{
  QString str = traceData->getLine(line);

  if (str.right(1) == "\n")
  {
    return str.size() - 1;
  }
  else
  {
    return str.size();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString Viewport::getTextForCopy(const QString& line, int textSize, bool* truncate)
{
  if (textSize + line.size() > maxCharacterCountForCopy)
  {
    if (truncate != nullptr)
    {
      *truncate = true;
    }

    return line.left(maxCharacterCountForCopy - textSize).append("...");
  }
  else
  {
    if (truncate != nullptr)
    {
      *truncate = false;
    }

    return line;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::highlightCursorLine(QList<QTextEdit::ExtraSelection>& selections)
{
  if (cursorLine >= startLine && cursorLine <= endLine)
  {
    QTextEdit::ExtraSelection highlight;
    highlight.cursor = textCursor();
    highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
    highlight.format.setForeground(QColor(textColor));
    highlight.format.setBackground(QColor(currentLineColor));
    highlight.cursor.clearSelection();
    selections << highlight;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::highlightDoubleClickSelection(QList<QTextEdit::ExtraSelection>& selections)
{
  if (!doubleClickSelection.isEmpty())
  {
    moveCursor(QTextCursor::Start);
    while (find(doubleClickSelection, QTextDocument::FindWholeWords))
    {
      QTextEdit::ExtraSelection extra;
      extra.cursor = textCursor();
      extra.format.setForeground(Qt::black);
      extra.format.setBackground(QColor(doubleClickSelectionColor));
      selections << extra;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::highlightSelection(QList<QTextEdit::ExtraSelection>& selections, const QStringList& lines)
{
  if (selectionExists())
  {
    int selectionStart = -1;
    int selectionEnd = -1;

    int visibleSelectionStartLine = selectionStartLine;
    int visibleSelectionStartColumn = selectionStartColumn;
    int visibleSelectionEndLine = selectionEndLine;
    int visibleSelectionEndColumn = selectionEndColumn;

    if (visibleSelectionStartLine > visibleSelectionEndLine)
    {
      std::swap(visibleSelectionStartLine, visibleSelectionEndLine);
      std::swap(visibleSelectionStartColumn, visibleSelectionEndColumn);
    }

    // Selection start
    if (visibleSelectionStartLine < startLine)
    {
      selectionStart = 0;
    }
    else if (visibleSelectionStartLine <= endLine)
    {
      int position = 0;

      for (int i = 0; i < visibleSelectionStartLine - startLine; ++i)
      {
        position += lines[i].size() + 1;
      }

      selectionStart = position + visibleSelectionStartColumn;
    }

    // Selection end
    if (visibleSelectionEndLine > endLine)
    {
      selectionEnd = toPlainText().size();
    }
    else if (visibleSelectionEndLine >= startLine)
    {
      int position = 0;

      for (int i = 0; i < visibleSelectionEndLine - startLine; ++i)
      {
        position += lines[i].size() + 1;
      }

      selectionEnd = position + visibleSelectionEndColumn;

      int size = document()->toPlainText().size();

      if (selectionEnd > size)
      {
        selectionEnd = size;
      }
    }

    // Add selection
    if (selectionStart != -1 && selectionEnd != -1)
    {
      QTextCursor tc = textCursor();
      QTextEdit::ExtraSelection highlight;

      tc.setPosition(selectionStart);
      tc.setPosition(selectionEnd, QTextCursor::KeepAnchor);
      highlight.cursor = tc;
      highlight.format.setForeground(Qt::black);
      highlight.format.setBackground(QColor(selectionColor));
      selections << highlight;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::processCopy()
{
  if (selectionExists())
  {
    if (selectionStartLine > selectionEndLine)
    {
      std::swap(selectionStartLine, selectionEndLine);
      std::swap(selectionStartColumn, selectionEndColumn);
    }
    else if (selectionStartLine == selectionEndLine)
    {
      if (selectionStartColumn > selectionEndColumn)
      {
        std::swap(selectionStartColumn, selectionEndColumn);
      }
    }

    if (selectionStartLine == selectionEndLine)
    {
      QApplication::clipboard()->setText(getTextForCopy(traceData->getLine(selectionStartLine).mid(selectionStartColumn, selectionEndColumn - selectionStartColumn)));
    }
    else
    {
      QString text = "";
      bool truncate = false;

      text.append(getTextForCopy(traceData->getLine(selectionStartLine).mid(selectionStartColumn), 0, &truncate));

      if (truncate)
      {
        QApplication::clipboard()->setText(text);
        return;
      }

      for (int i = selectionStartLine + 1; i < selectionEndLine; ++i)
      {
        text.append("\n");
        text.append(getTextForCopy(traceData->getLine(i), text.size(), &truncate));

        if (truncate)
        {
          QApplication::clipboard()->setText(text);
          return;
        }
      }

      text.append("\n");
      text.append(getTextForCopy(traceData->getLine(selectionEndLine).left(selectionEndColumn), text.size()));

      QApplication::clipboard()->setText(text);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::processDown()
{
  if (!shiftPressed && selectionExists())
  {
    clearSelection();
  }

  if (cursorLine < traceData->getSize() - 1)
  {
    if (cursorLine == endLine)
    {
      startLine++;
      cursorLine++;
      traceViewer->setVerticalScrollBarValue(startLine);
    }
    else
    {
      cursorLine++;
      setCursorColumn();
    }
  }

  if (shiftPressed)
  {
    selectionEndLine = cursorLine;

    int cursorLineSize = getEndIndexForLine(cursorLine);

    if (cursorColumn <= cursorLineSize)
    {
      selectionEndColumn = cursorColumn;
    }
    else
    {
      selectionEndColumn = cursorLineSize;
    }
  }

  horizontalScrollBarAdjustment = true;
  update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::processEnd()
{
  if (!shiftPressed && selectionExists())
  {
    clearSelection();
  }

  if (controlPressed)
  {
    startLine = traceData->getSize() - rowCount;

    if (startLine < 0)
    {
      startLine = 0;
    }

    cursorLine = traceData->getSize() - 1;
    cursorColumn = traceData->getLine(traceData->getSize() - 1).size();

    traceViewer->setVerticalScrollBarValue(traceData->getSize() - rowCount);
  }
  else
  {
    cursorColumn = getEndIndexForLine(cursorLine);
  }

  if (shiftPressed)
  {
    selectionEndLine = cursorLine;
    selectionEndColumn = cursorColumn;
  }

  horizontalScrollBarAdjustment = true;
  update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::processHome()
{
  if (!shiftPressed && selectionExists())
  {
    clearSelection();
  }

  if (controlPressed)
  {
    startLine = 0;
    cursorLine = 0;
    cursorColumn = 0;

    traceViewer->setVerticalScrollBarValue(0);
  }
  else
  {
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::StartOfLine);
    setTextCursor(tc);

    cursorColumn = 0;
  }

  if (shiftPressed)
  {
    selectionEndLine = cursorLine;
    selectionEndColumn = cursorColumn;
  }

  horizontalScrollBarAdjustment = true;
  update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::processLeft()
{
  if (!shiftPressed && selectionExists())
  {
    clearSelection();
  }

  // If not beginning of the line
  if (cursorColumn > 0)
  {
    QString currentLine = traceData->getLine(cursorLine);

    if (cursorColumn > currentLine.size())
    {
      cursorColumn = currentLine.size();
    }

    QChar startChar = currentLine.at(cursorColumn - 1);

    int skipCount = 0;

    if (controlPressed)
    {
      if (startChar != ' ' && BORDER_CHARS.contains(startChar))
      {
        skipCount = 1;
      }
      else
      {
        int i = cursorColumn;
        while (i > 0)
        {
          i--;
          skipCount++;

          if (i == 0 || (currentLine.at(i - 1) != ' ' && BORDER_CHARS.contains(currentLine.at(i - 1))) || (currentLine.at(i - 1) == ' ' && currentLine.at(i).isLetterOrNumber()))
          {
            break;
          }
        }
      }
    }
    else
    {
      skipCount = 1;
    }

    cursorColumn -= skipCount;
  }
  // If BOL but not the first line
  else if (cursorColumn == 0 && cursorLine - startLine > 0)
  {
    cursorLine--;
    cursorColumn = getEndIndexForLine(cursorLine);
  }
  // If BOL and first line in the view and not first data line, move the view up, and set the cursor position
  else if (cursorColumn == 0 && cursorLine - startLine == 0 && startLine > 0)
  {
    startLine--;
    cursorLine--;
    cursorColumn = getEndIndexForLine(cursorLine);
    traceViewer->setVerticalScrollBarValue(startLine);
  }

  if (shiftPressed)
  {
    selectionEndLine = cursorLine;
    selectionEndColumn = cursorColumn;
  }

  update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::processOutOfBoundsSelection()
{
  if (outOfBoundsScrollAmount < 0)
  {
    selectionStartLine = startLine;

    QTextBlock block = document()->findBlockByNumber(0);
    QRectF blockRect = blockBoundingGeometry(block).translated(contentOffset());
    QTextCursor tc = cursorForPosition(QPoint(mapFromGlobal(QCursor::pos()).x() - margin->width(), blockRect.top() + 1));
    selectionStartColumn = tc.positionInBlock();

    cursorLine = selectionStartLine;
    cursorColumn = selectionStartColumn;
  }
  else if (outOfBoundsScrollAmount > 0)
  {
    selectionEndLine = endLine;

    QTextBlock block = document()->findBlockByNumber(rowCount - 1);
    QRectF blockRect = blockBoundingGeometry(block).translated(contentOffset());
    QTextCursor tc = cursorForPosition(QPoint(mapFromGlobal(QCursor::pos()).x() - margin->width(), height() - blockRect.top() + 1));
    selectionEndColumn = tc.positionInBlock();

    cursorLine = selectionEndLine;
    cursorColumn = selectionEndColumn;
  }

  if (startLine == 0 || endLine == traceData->getSize() - 1)
  {
    stopSelectionTimer();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::processPageDown()
{
  if (!shiftPressed && selectionExists())
  {
    clearSelection();
  }

  int dataSize = traceData->getSize();

  cursorLine += rowCount;
  startLine += rowCount;
  if (cursorLine > dataSize - 1)
  {
    cursorLine = dataSize - 1;
    startLine = dataSize - rowCount;

    if (startLine < 0)
    {
      startLine = 0;
    }
  }

  if (shiftPressed)
  {
    selectionEndLine = cursorLine;

    int cursorLineSize = getEndIndexForLine(cursorLine);

    if (cursorColumn > cursorLineSize)
    {
      selectionEndColumn = cursorLineSize;
    }
    else
    {
      selectionEndColumn = cursorColumn;
    }
  }

  update();
  traceViewer->setVerticalScrollBarValue(startLine);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::processPageUp()
{
  if (!shiftPressed && selectionExists())
  {
    clearSelection();
  }

  cursorLine -= rowCount;
  startLine -= rowCount;
  if (cursorLine < 0)
  {
    cursorLine = 0;
    startLine = 0;
  }

  if (shiftPressed)
  {
    selectionEndLine = cursorLine;

    int cursorLineSize = getEndIndexForLine(cursorLine);

    if (cursorColumn > cursorLineSize)
    {
      selectionEndColumn = cursorLineSize;
    }
    else
    {
      selectionEndColumn = cursorColumn;
    }
  }

  update();
  traceViewer->setVerticalScrollBarValue(startLine);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::processRight()
{
  if (!shiftPressed && selectionExists())
  {
    clearSelection();
  }

  QString line = traceData->getLine(cursorLine);
  if (line.right(1) == "\n")
  {
    line = line.left(line.size() - 1);
  }

  // If not EOL
  if (cursorColumn < line.size())
  {
    QChar startChar = line.at(cursorColumn);

    int skipCount = 0;

    if (controlPressed)
    {
      if (startChar != ' ' && BORDER_CHARS.contains(startChar))
      {
        skipCount = 1;
      }
      else
      {
        int i = cursorColumn;
        while (i < line.size())
        {
          i++;
          skipCount++;

          if (i == line.size() || (line.at(i) != ' ' && BORDER_CHARS.contains(line.at(i))) || (line.at(i) == ' ' && line.at(i - 1).isLetterOrNumber()))
          {
            break;
          }
        }
      }
    }
    else
    {
      skipCount = 1;
    }

    cursorColumn += skipCount;
  }
  // If EOL but not the last line, move cursor to the next line
  else if (cursorColumn == line.size() && cursorLine - startLine < rowCount - 1 && cursorLine < traceData->getSize() - 1)
  {
    cursorLine++;
    cursorColumn = 0;
  }
  // if EOL and last line in the view and not last data line, move the view down, and set the cursor position
  else if (cursorColumn == line.size() && cursorLine - startLine == rowCount - 1 && cursorLine < traceData->getSize() - 1)
  {
    startLine++;
    cursorLine++;
    cursorColumn = 0;
    traceViewer->setVerticalScrollBarValue(startLine);
  }

  if (shiftPressed)
  {
    selectionEndLine = cursorLine;
    selectionEndColumn = cursorColumn;
  }

  update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::processSelectAll()
{
  if (traceData->getSize() > 0)
  {
    selectionStartLine = 0;
    selectionStartColumn = 0;
    selectionEndLine = traceData->getSize() - 1;
    selectionEndColumn = traceData->getLine(traceData->getSize() - 1).size();

    update();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::processUp()
{
  if (!shiftPressed && selectionExists())
  {
    clearSelection();
  }

  if (cursorLine > 0)
  {
    if (cursorLine - startLine == 0 && startLine > 0)
    {
      startLine--;
      cursorLine--;
      traceViewer->setVerticalScrollBarValue(startLine);
    }
    else
    {
      cursorLine--;
      setCursorColumn();
    }
  }

  if (shiftPressed)
  {
    selectionEndLine = cursorLine;

    int cursorLineSize = getEndIndexForLine(cursorLine);

    if (cursorColumn <= cursorLineSize)
    {
      selectionEndColumn = cursorColumn;
    }
    else
    {
      selectionEndColumn = cursorLineSize;
    }
  }

  horizontalScrollBarAdjustment = true;
  update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool Viewport::selectionExists() const
{
  return (selectionStartLine != -1 && selectionStartColumn != -1 && selectionEndLine != -1 && selectionEndColumn != -1);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::setCursorColumn()
{
  QString str = traceData->getLine(cursorLine);

  if (cursorColumn > str.size() - 1)
  {
    cursorColumn = str.size();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::startSelectionTimer(int amouot)
{
  outOfBoundsScrollAmount = amouot;
  selectionTimer->start(outOfBoundsScrollRate);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::stopSelectionTimer()
{
  selectionTimer->stop();
  outOfBoundsScrollAmount = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::updateCursorPosition(const QStringList& lines)
{
  QTextCursor tc = textCursor();

  if (cursorLine >= startLine && cursorLine <= endLine)
  {
    setCursorWidth(1);

    tc.movePosition(QTextCursor::Start);
    tc.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, cursorLine - startLine);

    if (cursorColumn < lines[cursorLine - startLine].size())
    {
      tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, cursorColumn);
    }
    else
    {
      tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, lines[cursorLine - startLine].size());
    }

    setTextCursor(tc);
  }
  else
  {
    setCursorWidth(0);

    tc.setPosition(0);
    setTextCursor(tc);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::updateMargin()
{
  int width = 0;

  for (int i = startLine; i <= endLine; ++i)
  {
    int fontWidth = fontMetrics().horizontalAdvance(traceData->getLineNumber(i, sourceLines));

    if (fontWidth > width)
    {
      width = fontWidth;
    }
  }

  if (width > 0)
  {
    int marginWidth = width + marginOffset;
    setViewportMargins(marginWidth, 0, 0, 0);
    margin->setFixedSize(QSize(marginWidth, this->height()));
    margin->update();
  }
}


//===================================================================================================================================================================================================//
// PUBLIC SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::onMarksChanged()
{
  updateMargin();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::setHorizontalScrollBarValue(int value)
{
  horizontalScrollBar()->setValue(value);

  horizontalScrollBarValue = value;
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::copyMarkedLines()
{
  QString text = "";
  bool truncate = false;

  QVector<Mark> markList = traceData->getMarks();

  for (int i = 0;  i < markList.size(); ++i)
  {
    text.append(getTextForCopy(traceData->getLine(markList.at(i).index), text.size(), &truncate));

    if (i < markList.size() - 1)
    {
      text.append("\n");
    }

    if (truncate)
    {
      QApplication::clipboard()->setText(text);
      return;
    }
  }

  QApplication::clipboard()->setText(text);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::onMarkToggled(int index)
{
  traceData->toggleMark(index);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Viewport::setOutOfBoundsSelection()
{
  traceViewer->scroll(outOfBoundsScrollAmount);
}