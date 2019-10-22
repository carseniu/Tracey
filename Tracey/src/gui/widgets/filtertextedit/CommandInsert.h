#pragma once

#include "src/gui/command/Command.h"

#include <QVector>

class FilterTextEdit;


class CommandInsert : public Command
{

public:
  CommandInsert(FilterTextEdit* filterTextEdit, const QString& text, const QVector<int>& operatorPositions = QVector<int>());

  void execute() override;
  void redo() override;
  void undo() override;

private:
  FilterTextEdit* filterTextEdit;
  QString text;
  QVector<int> operatorPositions;
  QString deletedText;
  QVector<int> deletedOperatorPositions;
  int position;
  int selectionStart;
  int selectionEnd;
  int start;

};