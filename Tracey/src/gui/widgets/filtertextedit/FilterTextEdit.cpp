#include "src/gui/widgets/filtertextedit/FilterTextEdit.h"

#include "src/gui/widgets/filtertextedit/CommandBackspace.h"
#include "src/gui/widgets/filtertextedit/CommandDelete.h"
#include "src/gui/widgets/filtertextedit/CommandInsert.h"
#include "src/gui/command/CommandManager.h"
#include "src/gui/widgets/filtertextedit/FilterHighlighter.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QMimeData>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterTextEdit::FilterTextEdit(QWidget* parent) :
  QPlainTextEdit(parent)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFont(QFont("Courier New", 10));
  setTabStopDistance(fontMetrics().averageCharWidth() * 4);

  filterHighlighter = new FilterHighlighter(this);
  filterHighlighter->setFont(font());

  commandManager = new CommandManager(this);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::addOperator(const QChar& filterOperator)
{
  commandManager->execute(new CommandInsert(this, QString(filterOperator), QVector<int> {0}));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::addOperatorPositions(const QVector<int>& operatorPositions, int start)
{
  for (const int operatorPosition : operatorPositions)
  {
    this->operatorPositions.append(operatorPosition + start);
  }

  std::sort(this->operatorPositions.begin(), this->operatorPositions.end());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::deleteText(int start, int n)
{
  removeOperators(start, start + n);
  shiftOperators(start, -n);

  QTextCursor tc = textCursor();
  tc.movePosition(QTextCursor::Start);
  tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, start);
  tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, n);
  tc.removeSelectedText();
  setTextCursor(tc);

  emit saveRequested();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QVector<int> FilterTextEdit::getOperatorPositions() const
{
  return operatorPositions;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QVector<int> FilterTextEdit::getOperatorPositions(int start, int n) const
{
  QVector<int> result;

  for (const int operatorPosition : operatorPositions)
  {
    if (operatorPosition >= start && operatorPosition < start + n)
    {
      result.append(operatorPosition - start);
    }
  }

  return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::highlightError(int position)
{
  filterHighlighter->highlightError(position);

  QTextCursor tc = textCursor();
  tc.setPosition(position + 1);
  setTextCursor(tc);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::insertText(const QString& text, const QVector<int>& operatorPositions, int start)
{
  QTextCursor tc = textCursor();
  tc.movePosition(QTextCursor::Start);
  tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, start);
  setTextCursor(tc);

  shiftOperators(start, text.size());
  addOperatorPositions(operatorPositions, start);
  insertPlainText(text);

  emit saveRequested();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::setOperatorPositions(const QVector<int>& operatorPositions)
{
  this->operatorPositions = operatorPositions;
  filterHighlighter->rehighlight();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::setSelectionAndCursorPosition(int position, int selectionStart, int selectionEnd)
{
  QTextCursor tc = textCursor();
  tc.movePosition(QTextCursor::Start);

  int selectionSize = selectionEnd - selectionStart;

  if (selectionSize > 0)
  {
    if (selectionEnd > position)
    {
      tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, selectionEnd);
      tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, selectionSize);
    }
    else
    {
      tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, selectionStart);
      tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, selectionSize);
    }
  }
  else
  {
    tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, position);
  }
  
  setTextCursor(tc);
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::contextMenuEvent(QContextMenuEvent* event)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::keyPressEvent(QKeyEvent* event)
{
  filterHighlighter->highlightError(-1);

  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
  {
    // Do nothing
  }
  else if (event->key() == Qt::Key_Left ||
    event->key() == Qt::Key_Right ||
    event->key() == Qt::Key_Up ||
    event->key() == Qt::Key_Down ||
    event->key() == Qt::Key_Home ||
    event->key() == Qt::Key_End ||
    (event->key() == Qt::Key_A && event->modifiers() == Qt::ControlModifier))
  {
    QPlainTextEdit::keyPressEvent(event);
  }
  else if (event->key() == Qt::Key_A && event->modifiers() == Qt::AltModifier)
  {
    addOperator('&');
  }
  else if (event->key() == Qt::Key_O && event->modifiers() == Qt::AltModifier)
  {
    addOperator('|');
  }
  else if (event->key() == Qt::Key_N && event->modifiers() == Qt::AltModifier)
  {
    addOperator('!');
  }
  else if (event->key() == Qt::Key_9 && event->modifiers() == Qt::AltModifier)
  {
    addOperator('(');
  }
  else if (event->key() == Qt::Key_0 && event->modifiers() == Qt::AltModifier)
  {
    addOperator(')');
  }
  else if (event->key() == Qt::Key_Delete)
  {
    processDelete();
  }
  else if (event->key() == Qt::Key_Backspace)
  {
    processBackspace();
  }
  else if (((event->key() == Qt::Key_C) && (event->modifiers() == Qt::ControlModifier)) ||
    ((event->key() == Qt::Key_Insert) && (event->modifiers() == Qt::ControlModifier)))
  {
    processCopy();
  }
  else if ((event->key() == Qt::Key_X) && (event->modifiers() == Qt::ControlModifier))
  {
    processCut();
  }
  else if (((event->key() == Qt::Key_V) && (event->modifiers() == Qt::ControlModifier)) ||
    ((event->key() == Qt::Key_Insert) && (event->modifiers() == Qt::ShiftModifier)))
  {
    processPaste();
  }
  else if ((event->key() == Qt::Key_Z) && (event->modifiers() == Qt::ControlModifier))
  {
    commandManager->undo();
  }
  else if ((event->key() == Qt::Key_Y) && (event->modifiers() == Qt::ControlModifier))
  {
    commandManager->redo();
  }
  else
  {  
    if (!event->text().isEmpty() && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier))
    {
      commandManager->execute(new CommandInsert(this, event->text()));
    }
  }
}

//==================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::processBackspace()
{
  if (textCursor().position() > 0)
  {
    commandManager->execute(new CommandBackspace(this));
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::processCopy()
{
  QTextCursor tc = textCursor();
  QString selection = tc.selectedText();

  if (!selection.isEmpty())
  {
    QClipboard* clipboard = QApplication::clipboard();
    QMimeData* mimeData = new QMimeData;

    QStringList positions;
    for (int i = tc.selectionStart(); i < tc.selectionEnd(); ++i)
    {
      if (operatorPositions.contains(i))
      {
        positions << QString::number(i - tc.selectionStart());
      }
    }

    if (!positions.isEmpty())
    {
      mimeData->setData("FILTER", positions.join(",").toLatin1());
    }
    mimeData->setText(selection);
    clipboard->setMimeData(mimeData);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::processCut()
{
  if (toPlainText().size() > 0)
  {
    processCopy();

    commandManager->execute(new CommandDelete(this));
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::processDelete ()
{
  if (!toPlainText().isEmpty())
  {
    commandManager->execute(new CommandDelete(this));
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::processPaste()
{
  const QClipboard* clipboard = QApplication::clipboard();
  const QMimeData* mimeData = clipboard->mimeData();
  QString text;
  QVector<int> positionsList;


  if (mimeData->hasText()) {
    // Get the text from the clipboard
    text = mimeData->text();

    // Remove newlines
    text.replace("\n", "");
    text.replace("\r", "");

    // Copy filter from Tracey
    if (mimeData->hasFormat("FILTER"))
    {
      QByteArray positionsData = mimeData->data("FILTER");
      QStringList positionsStrings = QString::fromLatin1(positionsData).split(",");

      // Add the new operator positions
      for (int i = 0; i < positionsStrings.size(); ++i)
      {
        positionsList.append(positionsStrings.at(i).toInt());
      }
    }

    commandManager->execute(new CommandInsert(this, text, positionsList));
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::removeOperators(int start, int end)
{
  for (int i = start; i < end; ++i)
  {
    for (int j = 0; j < operatorPositions.size(); ++j)
    {
      if (operatorPositions[j] == i)
      {
        operatorPositions.removeAt(j);
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterTextEdit::shiftOperators(int index, int delta)
{
  for (int i = 0; i < operatorPositions.size(); ++i)
  {
    if (operatorPositions[i] >= index)
    {
      operatorPositions[i] += delta;
    }
  }
}