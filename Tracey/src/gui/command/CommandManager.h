#pragma once

#include <QObject>

#include <QStack>

class Command;


class CommandManager : public QObject
{

public:
  CommandManager(QObject* parent);
  ~CommandManager();

  void execute(Command* command);
  void redo();
  void undo();

private:
  QStack<Command*> undoStack;
  QStack<Command*> redoStack;

};