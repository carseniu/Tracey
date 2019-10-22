#include "CommandManager.h"

#include "Command.h"


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
CommandManager::CommandManager(QObject* parent) :
  QObject(parent)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
CommandManager::~CommandManager()
{
  qDeleteAll(undoStack);
  qDeleteAll(redoStack);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void CommandManager::execute(Command* command)
{
  qDeleteAll(redoStack);
  redoStack.clear();

  command->execute();

  undoStack.push(command);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void CommandManager::redo()
{
  if (redoStack.isEmpty()) {
    return;
  }

  Command* command = redoStack.pop();
  command->redo();

  undoStack.push(command);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void CommandManager::undo()
{
  if (undoStack.isEmpty()) {
    return;
  }
  
  Command* command = undoStack.pop();
  command->undo();

  redoStack.push(command);
}