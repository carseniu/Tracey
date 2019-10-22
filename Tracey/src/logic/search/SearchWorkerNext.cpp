#include "src/logic/search/SearchWorkerNext.h"

#include "src/logic/core/TraceData.h"

#include <QDebug>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SearchWorkerNext::SearchWorkerNext(TraceData* traceData, int cursorLine, int cursorColumn, const QRegularExpression& regexp) :
  SearchWorker(),
  traceData(traceData),
  cursorLine(cursorLine),
  cursorColumn(cursorColumn),
  regexp(regexp),
  batchSize(10000)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SearchWorkerNext::~SearchWorkerNext()
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWorkerNext::process()
{
  int dataSize = traceData->getSize();
  int lineCount = 0;

  if (searchInLineAfterCursor(traceData->getLine(cursorLine), cursorColumn))
  {
    emit matchFound(cursorLine, foundStart, foundEnd);
    emit finished();

    return;
  }

  for (int i = cursorLine + 1; i < dataSize; i = i + batchSize)
  {
    QStringList lines = traceData->getLines(i, batchSize);

    for (int j = 0; j < lines.size(); ++j)
    {
      if (isCanceled())
      {
        emit finished();
        return;
      }

      lineCount++;

      if (searchInLine(lines.at(j)))
      {
        emit matchFound(i + j, foundStart, foundEnd);
        emit finished();

        return;
      }

      doUpdateProgress((double)lineCount / dataSize * 100);
    }
  }

  for (int i = 0; i <= cursorLine - 1; i = i + batchSize)
  {
    QStringList lines = traceData->getLines(i, batchSize);

    for (int j = 0; j < lines.size(); ++j)
    {
      if (isCanceled())
      {
        emit finished();
        return;
      }

      lineCount++;

      if (searchInLine(lines.at(j)))
      {
        emit matchFound(i + j, foundStart, foundEnd);
        emit finished();

        return;
      }

      doUpdateProgress((double)lineCount / dataSize * 100);
    }
  }

  if (searchInLineBeforeCursor(traceData->getLine(cursorLine), cursorColumn))
  {
    emit matchFound(cursorLine, foundStart, foundEnd);
    emit finished();

    return;
  }

  emit matchNotFound();
  emit updateReady(100, 0);
  emit finished();
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SearchWorkerNext::checkMatch(const QRegularExpressionMatch& match)
{
  bool hasMatch = match.hasMatch();

  if (hasMatch)
  {
    foundStart = match.capturedStart();
    foundEnd = match.capturedEnd();

    return true;
  }

  return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SearchWorkerNext::searchInLineAfterCursor(const QString& line, int cursorPosition)
{
  QRegularExpressionMatch match = regexp.match(line, cursorPosition);

  return checkMatch(match);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SearchWorkerNext::searchInLineBeforeCursor(const QString& line, int cursorPosition)
{
  QRegularExpressionMatch match = regexp.match(line.mid(0, cursorPosition));

  return checkMatch(match);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SearchWorkerNext::searchInLine(const QString& line)
{
  QRegularExpressionMatch match = regexp.match(line);

  return checkMatch(match);
}