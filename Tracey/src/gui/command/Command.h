#pragma once

#include <QObject>


class Command : public QObject
{

public:
  virtual void execute() = 0;
  virtual void redo() = 0;
  virtual void undo() = 0;

};