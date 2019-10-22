#pragma once

#include "src/gui/command/Command.h"

#include <QVector>

class FilterTextEdit;


class CommandDelete : public Command
{

public:
  CommandDelete(FilterTextEdit* filterTextEdit);

  void execute() override;
  void redo() override;
  void undo() override;

private:
  FilterTextEdit* filterTextEdit;
  QString deletedText;
  QVector<int> deletedOperatorPositions;
  int position;
  int selectionStart;
  int selectionEnd;
  int start;
  int deleteSize;

};