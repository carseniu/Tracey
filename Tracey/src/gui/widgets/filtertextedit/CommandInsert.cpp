#include "CommandInsert.h"

#include "FilterTextEdit.h"

#include <QDebug>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
CommandInsert::CommandInsert(FilterTextEdit* filterTextEdit, const QString& text, const QVector<int>& operatorPositions) :
  filterTextEdit(filterTextEdit),
  text(text),
  operatorPositions(operatorPositions)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void CommandInsert::execute()
{
  QTextCursor tc = filterTextEdit->textCursor();
  position = tc.position();
  selectionStart = tc.selectionStart();
  selectionEnd = tc.selectionEnd();
  start = tc.position();

  if (!tc.selectedText().isEmpty())
  {
    start = selectionStart;

    deletedText = tc.selectedText();
    deletedOperatorPositions = filterTextEdit->getOperatorPositions(selectionStart, selectionEnd - selectionStart);

    filterTextEdit->deleteText(selectionStart, selectionEnd - selectionStart);
  }

  filterTextEdit->insertText(text, operatorPositions, start);

  filterTextEdit->setFocus();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void CommandInsert::redo()
{
  if (!deletedText.isEmpty())
  {
    filterTextEdit->deleteText(selectionStart, selectionEnd - selectionStart);
  }

  filterTextEdit->insertText(text, operatorPositions, start);

  filterTextEdit->setFocus();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void CommandInsert::undo()
{
  filterTextEdit->deleteText(start, text.size());

  if (!deletedText.isEmpty())
  {
    filterTextEdit->insertText(deletedText, deletedOperatorPositions, start);
  }

  filterTextEdit->setSelectionAndCursorPosition(position, selectionStart, selectionEnd);

  filterTextEdit->setFocus();
}